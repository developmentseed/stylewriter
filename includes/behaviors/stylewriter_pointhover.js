// $Id: openlayers_behavior_zoomtolayer.js,v 1.1.2.7 2010/05/22 22:36:45 zzolo Exp $

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
 * Class: OpenLayers.Control.WMSGetFeatureInfo
 * The WMSGetFeatureInfo control uses a WMS query to get information about a point on the map.  The
 * information may be in a display-friendly format such as HTML, or a machine-friendly format such 
 * as GML, depending on the server's capabilities and the client's configuration.  This control 
 * handles click or hover events, attempts to parse the results using an OpenLayers.Format, and 
 * fires a 'getfeatureinfo' event with the click position, the raw body of the response, and an 
 * array of features if it successfully read the response.
 *
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.GridHover = OpenLayers.Class(OpenLayers.Control, {

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
    handler: null,
    
    /**
     * Property: hoverRequest
     * {<OpenLayers.Request>} contains the currently running hover request
     *     (if any).
     */
    hoverRequest: null,
    
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
        
        if(!this.format) {
            this.format = new OpenLayers.Format.WMSGetFeatureInfo(
                options.formatOptions
            );
        }
        
        if(this.drillDown === true) {
            this.hover = false;
        }

        this.handler = new OpenLayers.Handler.Hover(
               this, {
                   'move': this.cancelHover,
                   'pause': this.getInfoForHover
               },
               OpenLayers.Util.extend(this.handlerOptions.hover || {}, {
                   'delay': 30
               }));
    },

    /**
     * Method: activate
     * Activates the control.
     * 
     * Returns:
     * {Boolean} The control was effectively activated.
     */
    activate: function () {
        if (!this.active) {
            this.handler.activate();
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
        console.log(evt.target.src);
    },
   
    /**
     * Method: getInfoForHover
     * Pause callback for the hover handler
     *
     * Parameters:
     * evt - {Object}
     */
    getInfoForHover: function(evt) {
        this.target = evt.target;
        if ($(this.target).data('grid')) {
          grid = $(this.target).data('grid');
          // console.log(evt);
          offset = [
                Math.floor((evt.pageX - $(evt.target).offset().left) / 4),
                Math.floor((evt.pageY - $(evt.target).offset().top) / 4)];
          console.log(offset);
          console.log($(evt.target).position().left);
          if(grid[offset[1]][offset[0]]) {
            console.log(grid[offset[1]][offset[0]]);
          }
        }
        else {
          if (this.hoverRequest === null) {
            this.hoverRequest = $.ajax(
              {
                'url': evt.target.src.replace('png', 'json'), 
                context: this,
                success: this.readDone,
                error: function() {
                  console.log(this);
                  this.hoverRequest = null;
                },
                dataType: 'jsonp',
              }
            );
            this.events.triggerEvent("beforegetfeatureinfo", {xy: evt.xy});
          }
        }
    },

    readDone: function(data) {
      var g = data.features.split('|');
      var x = [];
      for (var i = 0; i < g.length; i++) {
        a = g[i].split(':');
        if (a.length == 1) {
          x.push(false);
        }
        else {
          for (j = 0; j < parseInt(a[0]); j++) {
            x.push(a[1]);
          }
        }
      }
      var grid = [];
      for (var i = 0; i < 64; i++) {
        grid[i] = x.splice(0, 64);
      }
      $(this.target).data('grid', grid);
      this.hoverRequest = null;
    },

    /**
     * Method: cancelHover
     * Cancel callback for the hover handler
     */
    cancelHover: function() {
        if (this.hoverRequest) {
            this.hoverRequest.abort();
            this.hoverRequest = null;
        }
    },

    /**
     * Method: triggerGetFeatureInfo
     * Trigger the getfeatureinfo event when all is done
     *
     * Parameters:
     * request - {XMLHttpRequest} The request object
     * xy - {<OpenLayers.Pixel>} The position on the map where the
     *     mouse event occurred.
     * features - {Array(<OpenLayers.Feature.Vector>)}
     */
    triggerGetFeatureInfo: function(request, xy, features) {
        this.events.triggerEvent("getfeatureinfo", {
            text: request.responseText,
            features: features,
            request: request,
            xy: xy
        });

        // Reset the cursor.
        OpenLayers.Element.removeClass(this.map.viewPortDiv, "olCursorWait");
    },
    CLASS_NAME: "OpenLayers.Control.GridHover"
});


/**
 * OpenLayers Zoom to Layer Behavior
 */
Drupal.behaviors.stylewriter_pointhover = function(context) {
  var layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_pointhover']) {
    map = data.openlayers;
    layer = map.getLayersBy('drupalID', 
      data.map.behaviors['stylewriter_pointhover'].layer)[0];
      h = new OpenLayers.Control.GridHover({
        layer: layer
      });
    map.addControl(h);
    h.activate();
  }
}
