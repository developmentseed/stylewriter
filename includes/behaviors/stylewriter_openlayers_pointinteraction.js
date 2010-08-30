/* Copyright (c) 2006-2008 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */


/**
 * @requires OpenLayers/Control.js
 * @requires OpenLayers/Handler/Click.js
 * @requires OpenLayers/Handler/Hover.js
 * @requires OpenLayers/Request.js
 */

/**
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.PointInteraction = OpenLayers.Class(OpenLayers.Control, {
    encode_base64: function(data) {
      var out = "", c1, c2, c3, e1, e2, e3, e4;
      // modified for function names
      var tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789___";
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
     * APIProperty: hover
     * {Boolean} Send GetFeatureInfo requests when mouse stops moving.
     *     Default is false.
     */
    hover: false,

    /**
     * APIProperty: drillDown
     * {Boolean} Drill down over all WMS layers in the map. When
     *     using drillDown mode, hover is not possible, and an infoFormat that
     *     returns parseable features is required. Default is false.
     */
    drillDown: false,

    /**
     * APIProperty: maxFeatures
     * {Integer} Maximum number of features to return from a WMS query. This
     *     sets the feature_count parameter on WMS GetFeatureInfo
     *     requests.
     */
    maxFeatures: 10,

    /** APIProperty: clickCallback
     *  {String} The click callback to register in the
     *      {<OpenLayers.Handler.Click>} object created when the hover
     *      option is set to false. Default is "click".
     */
    clickCallback: "click",
    
    /**
     * Property: layers
     * {Array(<OpenLayers.Layer.WMS>)} The layers to query for feature info.
     *     If omitted, all map WMS layers with a url that matches this <url> or
     *     <layerUrls> will be considered.
     */
    layers: null,

    /**
     * Property: queryVisible
     * {Boolean} If true, filter out hidden layers when searching the map for
     *     layers to query.  Default is false.
     */
    queryVisible: false,

    /**
     * Property: url
     * {String} The URL of the WMS service to use.  If not provided, the url
     *     of the first eligible layer will be used.
     */
    url: null,
    
    /**
     * Property: format
     * {<OpenLayers.Format>} A format for parsing GetFeatureInfo responses.
     *     Default is <OpenLayers.Format.WMSGetFeatureInfo>.
     */
    format: null,
    
    /**
     * Property: formatOptions
     * {Object} Optional properties to set on the format (if one is not provided
     *     in the <format> property.
     */
    formatOptions: null,

    /**
     * APIProperty: handlerOptions
     * {Object} Additional options for the handlers used by this control, e.g.
     * (start code)
     * {
     *     "click": {delay: 100},
     *     "hover": {delay: 300}
     * }
     * (end)
     */
    handlerOptions: null,
    
    /**
     * Property: handler
     * {Object} Reference to the <OpenLayers.Handler> for this control
     */
    handlers: null,
    
    /**
     * Property: hoverRequest
     * {<OpenLayers.Request>} contains the currently running hover request
     *     (if any).
     */
    hoverRequest: null,
    
    archive: {},
    /**
     * Constant: EVENT_TYPES
     *
     * Supported event types (in addition to those from <OpenLayers.Control>):
     * beforegetfeatureinfo - Triggered before the request is sent.
     *      The event object has an *xy* property with the position of the 
     *      mouse click or hover event that triggers the request.
     * getfeatureinfo - Triggered when a GetFeatureInfo response is received.
     *      The event object has a *text* property with the body of the
     *      response (String), a *features* property with an array of the
     *      parsed features, an *xy* property with the position of the mouse
     *      click or hover event that triggered the request, and a *request*
     *      property with the request itself. If drillDown is set to true and
     *      multiple requests were issued to collect feature info from all
     *      layers, *text* and *request* will only contain the response body
     *      and request object of the last request.
     */
    EVENT_TYPES: ["beforegetfeatureinfo", "getfeatureinfo"],

    /**
     * Constructor: <OpenLayers.Control.WMSGetFeatureInfo>
     *
     * Parameters:
     * options - {Object} 
     */
    initialize: function(options) {
        // concatenate events specific to vector with those from the base
        // this.EVENT_TYPES =
        //     OpenLayers.Control.WMSGetFeatureInfo.prototype.EVENT_TYPES.concat(
        //     OpenLayers.Control.prototype.EVENT_TYPES
        // );

        options = options || {};
        options.handlerOptions = options.handlerOptions || {};

        OpenLayers.Control.prototype.initialize.apply(this, [options]);
        
        if(this.drillDown === true) {
            this.hover = false;
        }
        this.format = new OpenLayers.Format.GeoJSON();

        this.handlers = {
          hover: new OpenLayers.Handler.Hover(
            this, {
                'move': this.cancelHover,
                'pause': this.getInfoForHover
            },
            OpenLayers.Util.extend(this.handlerOptions.hover || {}, {
                'delay': 30
              }
            )
          ),
          click: new OpenLayers.Handler.Click(
            this, {
                'click': this.getInfoForClick
            }
          )};
    },

    /** 
     * Method: setMap
     * Set the map property for the control. 
     * 
     * Parameters:
     * map - {<OpenLayers.Map>} 
     */
    setMap: function(map) {
        this.handlers.hover.setMap(map);
        this.handlers.click.setMap(map);
        OpenLayers.Control.prototype.setMap.apply(this, arguments);
    },

    /**
     * Method: activate
     * Activates the control.
     * 
     * Returns:
     * {Boolean} The control was effectively activated.
     */
    activate: function() {
        if (!this.active) {
            this.handlers.hover.activate();
            this.handlers.click.activate();
        }
        return OpenLayers.Control.prototype.activate.apply(
            this, arguments
        );
    },

    /**
     * Method: deactivate
     * Deactivates the control.
     * 
     * Returns:
     * {Boolean} The control was effectively deactivated.
     */
    deactivate: function () {
        return OpenLayers.Control.prototype.deactivate.apply(
            this, arguments
        );
    },
    
    /**
     * Method: getInfoForClick 
     * Called on click
     *
     * Parameters:
     * evt - {<OpenLayers.Event>} 
     */
    getInfoForClick: function(evt) {
      this.target = evt.target;
      if (this.archive[$(this.target).attr('src')]) {
        grid = this.archive[$(this.target).attr('src')]
        if (grid === true || grid == undefined) { // is downloading
          return;
        }
        point = this.getPoint(evt, grid);
        point && this.callbacks['click'](point, this.layer);
      }
      else {
        if (!this.archive[$(evt.target).attr('src')]) {
          this.target.req = true;
          try {
            this.archive[$(evt.target).attr('src')] = true;
            this.target.hoverRequest = this.reqTile(evt);
          } catch(err) {
            this.archive[$(evt.target).attr('src')] = false;
          }
        }
      }
    },

    reqTile: function(evt) {
      return $.ajax(
        {
          'url': $(evt.target).attr('src').replace('png', 'json'), 
          context: this,
          success: $.proxy(this.readDone, this),
          error: function() {},
          dataType: 'jsonp',
          jsonpCallback: "f" +this.encode_base64($(evt.target).attr('src'))
        }
      );
    },

    getPoint: function(evt, grid) {
      lonLat = this.map.getLonLatFromPixel(evt.xy);
      lonLat.transform(new OpenLayers.Projection('EPSG:900913'), new OpenLayers.Projection('EPSG:4326'));
      here = new OpenLayers.Geometry.Point(lonLat.lon, lonLat.lat);
      for(var i = 0; i < grid.length; i++) {
        if(grid[i].geometry.containsPoint(here)) {
          return grid[i].attributes;
        }
      }
    },
   
    /**
     * Method: getInfoForHover
     * Pause callback for the hover handler
     *
     * Parameters:
     * evt - {Object}
     *
     * This can be called, at max, once every 250 ms
     */
    getInfoForHover: function(evt) {
        this.target = evt.target;
        if (this.archive[$(this.target).attr('src')]) {
          grid = this.archive[$(this.target).attr('src')]
          if (grid === true || grid == undefined) { // is downloading
            return;
          }
          point = this.getPoint(evt, grid);
          point ? this.callbacks['over'](point, this.layer) :
            this.callbacks['out']();
        }
        else {
          this.callbacks['out']({}, this.layer);
          if (!this.archive[$(evt.target).attr('src')]) {
            this.target.req = true;
            try {
              this.archive[$(evt.target).attr('src')] = true;
              this.target.hoverRequest = this.reqTile(evt);
            } catch(err) {
              this.archive[$(evt.target).attr('src')] = false;
            }
          }
        }
    },

    readDone: function(data) {
      var l = new OpenLayers.Layer.Vector('new vec');
      l.addFeatures(this.format.read(data));
      this.layer.map.addLayer(l);
      this.archive[$(this.target).attr('src')] = this.format.read(data);
    },
    CLASS_NAME: "OpenLayers.Control.PointInteraction"
});
