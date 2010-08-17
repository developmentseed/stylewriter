/* Copyright (c) 2006-2008 MetaCarta, Inc., published under the Clear BSD
 * license.  See http://svn.openlayers.org/trunk/openlayers/license.txt for the
 * full text of the license. */

/**
 * Class: OpenLayers.Control.VirtualPoint
 * 
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.VirtualPoint = OpenLayers.Class(OpenLayers.Control, {

   /**
     * APIProperty: hover
     * {Boolean} Send GetFeatureInfo requests when mouse stops moving.
     *     Default is false.
     */
    hover: true,

    /** APIProperty: clickCallback
     *  {String} The click callback to register in the
     *      {<OpenLayers.Handler.Click>} object created when the hover
     *      option is set to false. Default is "click".
     */
    clickCallback: "click",

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
        options = options || {};
        options.handlerOptions = options.handlerOptions || {};

        OpenLayers.Control.prototype.initialize.apply(this, [options]);
        
        /*
        if(!this.format) {
            this.format = new OpenLayers.Format.JSONP(
                options.formatOptions
            );
        }
        */
       
        if(this.hover) {
            this.handler = new OpenLayers.Handler.Hover(
                   this, {
                       'move': this.cancelHover,
                       'pause': this.getInfoForHover
                   },
                   OpenLayers.Util.extend(this.handlerOptions.hover || {}, {
                       'delay': 250
                   }));
        } else {
            var callbacks = {};
            callbacks[this.clickCallback] = this.getInfoForClick;
            this.handler = new OpenLayers.Handler.Click(
                this, callbacks, this.handlerOptions.click || {});
        }
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
        this.events.triggerEvent("beforegetfeatureinfo", {xy: evt.xy});
        // Set the cursor to "wait" to tell the user we're working on their
        // click.
        OpenLayers.Element.addClass(this.map.viewPortDiv, "olCursorWait");
        this.request(evt.xy, {});
    },
   
    /**
     * Method: getInfoForHover
     * Pause callback for the hover handler
     *
     * Parameters:
     * evt - {Object}
     */
    getInfoForHover: function(evt) {
      // This loop will be run many times - it must be fast
      // TODO: rewrite to box features into areas of the screen on 
      // addition to reduce the performance hit of calculating
      for(var i = 0; i < this.layer.features.length; i++) {
        if(this.layer.features[i].cache_px === null) {
          this.layer.features[i].cache_px = this.map.getPixelFromLonLat(
            new OpenLayers.LonLat(
              this.layer.features[i].geometry.x,
              this.layer.features[i].geometry.y));
        }
        // TODO: write inner test which determines whether cursor is
        // in a circle
        // TODO: deal with multiple-sized features
        if(this.layer.features[i].cache_px &&
          (Math.abs(this.layer.features[i].cache_px.x - evt.xy.x) 
          < this.FEATURE_SIZE) && 
          (Math.abs(this.layer.features[i].cache_px.y - evt.xy.y) 
          < this.FEATURE_SIZE)) {
             this.hoverOver(evt);
             return;
        }
      }
      this.hoverOut(evt);
      this.events.triggerEvent("beforegetfeatureinfo", {xy: evt.xy});
        //this.request(evt.xy, {hover: true});
      return;
    },

    /**
     * Method: cancelHover
     * Cancel callback for the hover handler
     */
    cancelHover: function() {
        for(var i = 0; i < this.layer.features.length; i++) {
          this.layer.features[i].cache_px = null;
        }

        // if (this.hoverRequest) {
        //     this.hoverRequest.abort();
        //     this.hoverRequest = null;
        // }
    },

    /**
     * Method: findLayers
     * Internal method to get the layers, independent of whether we are
     *     inspecting the map or using a client-provided array
     */
    findLayers: function() {
        var candidates = this.layers || this.map.layers;
        var layers = [];
        var layer, url;
        for(var i=0, len=candidates.length; i<len; ++i) {
            layer = candidates[i];
            if(layer instanceof OpenLayers.Layer.StyleWriter &&
               (!this.queryVisible || layer.getVisibility())) {
                url = layer.url instanceof Array ? layer.url[0] : layer.url;
                // if the control was not configured with a url, set it
                // to the first layer url
               layers.push(layer);
            }
        }
        return layers;
    },

    /**
     * Method: request
     * Sends a GetFeatureInfo request to the WMS
     * 
     * Parameters:
     * clickPosition - {<OpenLayers.Pixel>} The position on the map where the
     *     mouse event occurred.
     * options - {Object} additional options for this method.
     * 
     * Valid options:
     * - *hover* {Boolean} true if we do the request for the hover handler
     */
    request: function(clickPosition, options) {
        var layers = this.findLayers();
        if(layers.length == 0) {
            // Reset the cursor.
            OpenLayers.Element.removeClass(this.map.viewPortDiv, "olCursorWait");
            return;
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
    
    /**
     * Method: handleResponse
     * Handler for the GetFeatureInfo response.
     * 
     * Parameters:
     * xy - {<OpenLayers.Pixel>} The position on the map where the
     *     mouse event occurred.
     * request - {XMLHttpRequest} The request object.
     */
    handleResponse: function(xy, request) {
        var doc = request.responseXML;
        if(!doc || !doc.documentElement) {
            doc = request.responseText;
        }
        var features = this.format.read(doc);
        if (this.drillDown === false) {
            this.triggerGetFeatureInfo(request, xy, features);
        } else {
            this._requestCount++;
            this._features = (this._features || []).concat(features);
            if (this._requestCount === this._numRequests) {
                this.triggerGetFeatureInfo(request, xy, this._features.concat()); 
                delete this._features;
                delete this._requestCount;
                delete this._numRequests;
            }
        }
    },
    CLASS_NAME: "OpenLayers.Control.VirtualPoint"
});
