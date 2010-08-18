/**
 * @file
 * Layer handler for Stylewriter layers
 */

/**
 * @requires OpenLayers/Strategy.js
 * @requires OpenLayers/Console.js
 */

/**
 * Class: OpenLayers.Strategy.Grid
 * Base class for layers that use a lattice of tiles.  Create a new grid
 * layer with the <OpenLayers.Layer.Grid> constructor.
 *
 * Inherits from:
 *  - <OpenLayers.Layer.HTTPRequest>
 */
OpenLayers.Strategy.Grid = OpenLayers.Class(OpenLayers.Strategy, {

    urlsafe_encode_base64: function(data) {
      return this.encode_base64(data).replace('/', '_').replace('+', '-');
    },

    encode_base64: function(data) {
      var out = "", c1, c2, c3, e1, e2, e3, e4;
      var tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
      for (var i = 0; i < data.length; ) {
         c1 = data.charCodeAt(i++);
         c2 = data.charCodeAt(i++);
         c3 = data.charCodeAt(i++);
         e1 = c1 >> 2;
         e2 = ((c1 & 3) << 4) + (c2 >> 4);
         e3 = ((c2 & 15) << 2) + (c3 >> 6);
         e4 = c3 & 63;
         if (isNaN(c2))
           e3 = e4 = 64;
         else if (isNaN(c3))
           e4 = 64;
         out += tab.charAt(e1) + tab.charAt(e2) + tab.charAt(e3) + tab.charAt(e4);
      }
      return out;
    },
    /**
     * Property: tileSize
     * {<OpenLayers.Size>}
     */
    tileSize: null,

    /**
     * Property: grid
     * {Array(Array(<OpenLayers.Tile>))} This is an array of rows, each row is
     *     an array of tiles.
     */
    grid: null,

    /**
     * APIProperty: buffer
     * {Integer} Used only when in gridded mode, this specifies the number of
     *           extra rows and colums of tiles on each side which will
     *           surround the minimum grid tiles to cover the map.
     */
    buffer: 1,

    /**
     * APIProperty: loadedBounds
     * {OpenLayers.Bounds} Extent actually loaded in tile coordinates.
     */
    loadedBounds: null,

    /**
     * APIProperty: numLoadingTiles
     * {Integer} How many tiles are still loading?
     */
    numLoadingTiles: 0,

    /**
     * Property: zoom
     * {Integer} Last zoom; detect zoom changes
     */
    zoom: null,

    /**
     * Property: geometryFeatureMap
     * {Object} Maps geometries to features
     */
    geometryFeatureMap: {},

    /**
     * Properties: loadedTiles
     * {Object} All currently loaded tiles.
     */
    tiles: {},
    
    /**
     * Constructor: OpenLayers.Strategy.Grid
     * Create a new Grid strategy.
     *
     * Parameters:
     * options - {Object} Optional object whose properties will be set on the
     *     instance.
     */
    initialize: function(options) {
        OpenLayers.Strategy.prototype.initialize.apply(this, [options]);
    },

    /**
     * APIMethod: destroy
     * Deconstruct the layer and clear the grid.
     */
    destroy: function() {
        this.clearGrid();
        this.grid = null;
        this.tileSize = null;
        OpenLayers.Strategy.prototype.destroy.apply(this, arguments);
    },


    /**
     * Method: activate
     * Set up strategy with regard to reading new batches of remote data.
     *
     * Returns:
     * {Boolean} The strategy was successfully activated.
     */
    activate: function() {
        var activated = OpenLayers.Strategy.prototype.activate.call(this);
        if(activated) {
            this.layer.events.on({
                "moveend": this.update,
                "refresh": this.update,
                scope: this
            });
            if(this.layer.visibility == true || this.preload) {
                this.update();
            } else {
                this.layer.events.on({
                    "visibilitychanged": this.load,
                    scope: this
                });
            }
        }
        
        return activated;
    },

    /**
     * Method: deactivate
     * Tear down strategy with regard to reading new batches of remote data.
     *
     * Returns:
     * {Boolean} The strategy was successfully deactivated.
     */
    deactivate: function() {
        var deactivated = OpenLayers.Strategy.prototype.deactivate.call(this);
        if(deactivated) {
            this.layer.events.un({
                "moveend": this.update,
                "refresh": this.update,
                "visibilitychanged": this.load,
                scope: this
            });
        }
        return deactivated;
    },

    /**
     * Method: triggerRead
     *
     * Returns:
     * {<OpenLayers.Protocol.Response>} The protocol response object
     *      returned by the layer protocol.
     */
    loadTile: function(x, y, z, bounds) {
        var url = OpenLayers.String.format(this.layer.options.base_url, {
          x: x,
          y: y,
          z: z,
          format: 'json',
          mapfile: this.urlsafe_encode_base64(this.layer.options.mapfile),
        });
        $.ajax(
          {
            'url': url, 
            context: this,
            success: this.readDone,
            error: function() {
              alert("there was an error!");
            },
            dataType: 'jsonp',
          }
        );
    },

    /**
     * Method: update
     * Callback function called on "moveend" or "refresh" layer events.
     */
    update: function() {
        var bounds = this.layer.map.getExtent();
        if (bounds == null) return;

        // Find tiles to load
        var map = this.layer.map
        var resolution = map.getResolution();
        var tileSize = map.getTileSize();
        var curZoom = map.zoom
        var extent = this.layer.maxExtent
        var tilelon = resolution * tileSize.w;
        var tilelat = resolution * tileSize.h;

        //Get tile bounding box
        var left = Math.floor((bounds.left - extent.left)/tilelon) - this.buffer;
        var right = Math.ceil((bounds.right - extent.left)/tilelon) + this.buffer;
        var top = Math.floor((extent.top - bounds.top)/tilelat) - this.buffer;
        var bottom = Math.ceil((extent.top - bounds.bottom)/tilelat) + this.buffer;

        if (curZoom != this.zoom) {
            this.layer.destroyFeatures();
            this.geometryFeatureMap = {};
            this.tiles = {};
//             this.loadedBounds = null;
            this.zoom = curZoom
        }

        for (var x=left; x<right; x++) {
            for (var y=top; y<bottom; y++) {
                if (!([x, y] in this.tiles)) {
                    var bl = x * tilelon + extent.left;
                    var bt = extent.top - y * tilelat;
                    this.loadTile(x, y, curZoom,
                        new OpenLayers.Bounds(bl, bt-tilelat, bl+tilelon, bt));
                }
            }
        }
    },

    readDone: function(resp) {
        this.scope.merge(resp, this.options);
    },

    /* Method: merge
     * Given a list of features, determine which ones to add to the layer.
     *     If the layer projection differs from the map projection, features
     *     will be transformed from the layer projection to the map projection.
     *
     * Parameters:
     * resp - {<OpenLayers.Protocol.Response>} The response object passed
     *      by the protocol.
     */
    merge: function(resp, options) {
        if (options.z != this.zoom) return; //Zoom has changed while read was active //TODO: Cancel these reads!
        var bounds = options.bounds;
        var features = resp.features;
        var filtered = [];
        if(features && features.length > 0) {
            var remote = this.layer.projection;
            var local = this.layer.map.getProjectionObject();
            var update = !local.equals(remote);
            for(var i=0, len=features.length; i<len; ++i) {
                var geom = features[i].geometry;
                features[i].usage_count = 1;
                if (geom) {
                    if (update) geom.transform(remote, local);
                    if (geom in this.geometryFeatureMap) {
                        var existing_feature = this.geometryFeatureMap[geom];
                        existing_feature.usage_count += 1;
                        features[i].destroy(); //Free unused feature
                    } else {
                        filtered.push(features[i]);
                        this.geometryFeatureMap[geom] = features[i];
                    }
                }
            }
            this.tiles[[options.x, options.y]] = true;
            this.layer.addFeatures(filtered);
        }
        this.response = null;
        //TODO: Trigger after last tile
        //this.layer.events.triggerEvent("loadend");
    },
                                            
    CLASS_NAME: "OpenLayers.Strategy.Grid"
});

/**
 * Openlayer layer handler for Stylewriter layer
 */
Drupal.openlayers.layer.stylewriter = function (title, map, options) {
  options.projection = 'EPSG:'+options.projection;

  /** add interaction layer **/
  var in_options = {
    // 'internalProjection': new OpenLa,
    'externalProjection': new OpenLayers.Projection("EPSG:4326")
  };
  format = new OpenLayers.Format.GeoJSON(in_options);
  strategy = new OpenLayers.Strategy.Grid();
  vectors = new OpenLayers.Layer.Vector("Vector", {
      mapfile: options.mapfile,
      base_url: options.base_url,
      strategies: [strategy]
  });

  return vectors;
};
