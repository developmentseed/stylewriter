/* Copyright (c) 2006-2009 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */

/**
 * @requires OpenLayers/Layer/Grid.js
 * @requires OpenLayers/Tile/Image.js
 */

// Based on public domain code by Tyler Akins <http://rumkin.com/>
// Original code at http://rumkin.com/tools/compression/base64.php
var Base64 = (function() {
  function urlsafe_encode_base64(data) {
    return this.encode(data).replace('/', '_').replace('+', '-');
  }

  function encode_base64(data) {
    var out = "", c1, c2, c3, e1, e2, e3, e4;
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
  }

  function decode_base64(data) {
    var out = "", c1, c2, c3, e1, e2, e3, e4;
    for (var i = 0; i < data.length; ) {
      e1 = tab.indexOf(data.charAt(i++));
      e2 = tab.indexOf(data.charAt(i++));
      e3 = tab.indexOf(data.charAt(i++));
      e4 = tab.indexOf(data.charAt(i++));
      c1 = (e1 << 2) + (e2 >> 4);
      c2 = ((e2 & 15) << 4) + (e3 >> 2);
      c3 = ((e3 & 3) << 6) + e4;
      out += String.fromCharCode(c1);
      if (e3 != 64)
        out += String.fromCharCode(c2);
      if (e4 != 64)
        out += String.fromCharCode(c3);
    }
    return out;
  }
  var tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  return { encode:encode_base64, decode:decode_base64, urlsafe_encode:urlsafe_encode_base64 };
})();

/** 
 * Class: OpenLayers.Layer.StyleWriter
 * The StyleWriter class is designed to make it easier for people who have tiles
 * arranged by a standard XYZ grid. 
 */
OpenLayers.Layer.StyleWriter = OpenLayers.Class(OpenLayers.Layer.Grid, {
    
    /**
     * APIProperty: isBaseLayer
     * Default is true, as this is designed to be a base tile source. 
     */
    isBaseLayer: true,
    
    /**
     * APIProperty: sphericalMecator
     * Whether the tile extents should be set to the defaults for 
     *    spherical mercator. Useful for things like OpenStreetMap.
     *    Default is false, except for the OSM subclass.
     */
    sphericalMercator: false,

    /**
     * Constructor: OpenLayers.Layer.StyleWriter
     *
     * Parameters:
     * name - {String}
     * url - {String}
     * options - {Object} Hashtable of extra options to tag onto the layer
     */
    initialize: function(name, url, options) {
        if (options && options.sphericalMercator || this.sphericalMercator) {
            options = OpenLayers.Util.extend({
                maxExtent: new OpenLayers.Bounds(
                    -128 * 156543.0339,
                    -128 * 156543.0339,
                    128 * 156543.0339,
                    128 * 156543.0339
                ),
                maxResolution: 156543.0339,
                numZoomLevels: 19,
                units: "m",
                projection: "EPSG:900913"
            }, options);
        }
        url = url || this.url;
        name = name || this.name;
        var newArguments = [name, url, {}, options];
        OpenLayers.Layer.Grid.prototype.initialize.apply(this, newArguments);
    },
    
    /**
     * APIMethod: clone
     * Create a clone of this layer
     *
     * Parameters:
     * obj - {Object} Is this ever used?
     * 
     * Returns:
     * {<OpenLayers.Layer.Grid>} An exact clone of this OpenLayers.Layer.Grid
     */
    clone: function (obj) {
        
        if (obj == null) {
            obj = new OpenLayers.Layer.StyleWriter(this.name,
                                            this.url,
                                            this.getOptions());
        }

        //get all additions from superclasses
        obj = OpenLayers.Layer.HTTPRequest.prototype.clone.apply(this, [obj]);

        // copy/set any non-init, non-simple values here
        if (this.tileSize != null) {
            obj.tileSize = this.tileSize.clone();
        }
        
        // we do not want to copy reference to grid, so we make a new array
        obj.grid = [];

        return obj;
    },    

    /**
     * Method: getUrl
     *
     * Parameters:
     * bounds - {<OpenLayers.Bounds>}
     *
     * Returns:
     * {String} A string with the layer's url and parameters and also the
     *          passed-in bounds and appropriate tile size specified as
     *          parameters
     */
    getURL: function (bounds) {
        var res = this.map.getResolution();
        var x = Math.round((bounds.left - this.maxExtent.left) 
            / (res * this.tileSize.w));
        var y = Math.round((this.maxExtent.top - bounds.top) 
            / (res * this.tileSize.h));
        var z = this.map.getZoom();

        var url = this.url;
        var s = '' + x + y + z;

        if (this.minzoom !== null) {
          z += parseInt(this.minzoom);
        }

        if (url instanceof Array)
        {
            url = this.selectUrl(s, url);
        }
        
        var path = OpenLayers.String.format(url, {'x': x, 'y': y, 'z': z});

        return path;
    },
    
    /**
     * Method: addTile
     * addTile creates a tile, initializes it, and adds it to the layer div. 
     * 
     * Parameters:
     * bounds - {<OpenLayers.Bounds>}
     * position - {<OpenLayers.Pixel>}
     * 
     * Returns:
     * {<OpenLayers.Tile.Image>} The added OpenLayers.Tile.Image
     */
    addTile:function(bounds,position) {
        return new OpenLayers.Tile.Image(this, position, bounds, 
                                         null, this.tileSize);
    },
     
    /* APIMethod: setMap
     * When the layer is added to a map, then we can fetch our origin 
     *    (if we don't have one.) 
     * 
     * Parameters:
     * map - {<OpenLayers.Map>}
     */
    setMap: function(map) {
        OpenLayers.Layer.Grid.prototype.setMap.apply(this, arguments);
        if (!this.tileOrigin) { 
            this.tileOrigin = new OpenLayers.LonLat(this.maxExtent.left,
                                                this.maxExtent.bottom);
        }                                       
    },

    CLASS_NAME: "OpenLayers.Layer.StyleWriter"
});
