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
 * Class: OpenLayers.Control.GridHover
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.PolygonInteraction = OpenLayers.Class(OpenLayers.Control, {

    // TODO: unify in a javascript file instead of scattering
    encode_base64: function(data) {
      var out = '', c1, c2, c3, e1, e2, e3, e4;
      var tab = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=';
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

    /** APIProperty: clickCallback
     *  {String} The click callback to register in the
     *      {<OpenLayers.Handler.Click>} object created when the hover
     *      option is set to false. Default is "click".
     */
    clickCallback: 'click',

    /**
     * Property: queryVisible
     * {Boolean} If true, filter out hidden layers when searching the map for
     *     layers to query.  Default is false.
     */
    queryVisible: false,

    /**
     * Property: format
     * {<OpenLayers.Format>} A format for parsing GetFeatureInfo responses.
     *     Default is <OpenLayers.Format.WMSGetFeatureInfo>.
     */
    format: null,

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
    EVENT_TYPES: ['beforegetfeatureinfo', 'getfeatureinfo'],

    /**
     * Constructor: <OpenLayers.Control.PolygonInteraction>
     *
     * Parameters:
     * options - {Object}
     */
    initialize: function(options) {
        options = options || {};
        options.handlerOptions = options.handlerOptions || {};

        OpenLayers.Control.prototype.initialize.apply(this, [options]);

        this.handlers = {
          hover: new OpenLayers.Handler.Hover(
            this, {
                'move': this.cancelHover,
                'pause': this.getInfoForHover
            },
            OpenLayers.Util.extend(this.handlerOptions.hover || {}, {
                'delay': ($.browser.msie) ? 40 : 10
              }
            )
          ),
          click: new OpenLayers.Handler.Click(
            this, {
                'click': this.getInfoForClick
            }
          )
        };
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
    deactivate: function() {
        return OpenLayers.Control.prototype.deactivate.apply(
            this, arguments
        );
    },


    getGridCell: function(evt) {
      var etarget = evt.target || evt.srcElement;
      var code_string = this.fString($(etarget).attr('src'))
      grid = this.archive[code_string];

      if (grid === true) { // is downloading
        return;
      }

      pX = evt.pageX || evt.clientX;
      pY = evt.pageY || evt.clientY;

      offset = [
        Math.floor((pX - $(etarget).offset().left) / 4),
        Math.floor((pY - $(etarget).offset().top) / 4)];

      return grid[offset[1]] && grid[offset[1]][offset[0]];
    },

    /**
     * Generate a function-safe string from a URL string
     */
    fString: function(src) {
      if ( !src) return;
      pts = src.split('/').slice(-4).join('_').replace(/=/g, '_').split('.');
      pts.pop();
      return pts.pop();
    },

    /**
     * Method: reqTile
     * Requests a single tile
     *
     * Parameters:
     * evt - {Browser Event}
     */
    reqTile: function(etarget, layer) {
      var code_string = this.fString($(etarget).attr('src'))

      this.archive[code_string] = true;

      etarget.hoverRequest = $.jsonp(
        {
          'url': $(etarget).attr('src').replace(
            'png',
            this.encode_base64(layer.options.join_field) + '.grid.json'),
          context: this,
          success: this.readDone,
          error: function() {},
          callback: code_string,
          callbackParameter: 'callback'
        }
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
      this.target = evt.target || evt.srcElement;
      var code_string = this.fString($(this.target).attr('src'))
      if (this.archive[$(this.target).attr('src')]) {
        key = this.getGridCell(evt);
        key && this.callbacks['click'](this.layer.options.keymap[this.key], this.layer);
      }
      else {
        if (!this.archive[code_string]) {
          this.target.req = true;
          try {
            // TODO: recall getInfoForClick
            this.target.hoverRequest = this.reqTile(this.target);
          } catch (err) {
            this.archive[code_string] = false;
          }
        }
      }
    },

    layerOnTop: function(tile) {
      layers = this.map.getLayersBy('baselayer', 0);
      return $(layers).filter(function(i) {
        return (layers[i].visibility == 1) && (layers[i].options.symbolizer == 'polygon');
      })[0];
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
        this.target = evt.target || evt.srcElement;
        var layer = this.layerOnTop(this.target);
        if (layer) {
          var code_string = this.fString($(this.target).attr('src'))
          if (this.archive[code_string]) {
            key = this.getGridCell(evt);
            if (key) {
              if (key !== this.key) {
                this.key = key;
                this.callbacks['out'](layer.options.keymap[this.key], layer, evt);
                if (layer.options.keymap[key]) {
                  this.callbacks['over'](layer.options.keymap[this.key], layer, evt);
                }
                else {
                }
              }
            }
            else {
              this.callbacks['out'](layer.options.keymap[this.key], layer, evt);
            }
          }
          else {
            this.callbacks['out']({}, layer);
            if (!this.archive[code_string]) {
              this.target.req = true;
              try {
                this.target.hoverRequest = this.reqTile(this.target, layer);
              } catch (err) {
                this.archive[code_string] = false;
              }
            }
          }
        }
    },

    readDone: function(data) {
      var g = data.features.split('|');
      var x = [];
      // Quick RLE decompression, this could be faster
      for (var i = 0; i < g.length; i++) {
        a = g[i].split(':');
        l = parseInt(a[0], 10);
        if (a.length == 1) {
          for (j = 0; j < l; j++) {
            x.push(false);
          }
        }
        else {
          for (j = 0; j < l; j++) {
            x.push(a[1]);
          }
        }
      }
      var grid = [];
      for (var i = 0; i < 64; i++) {
        grid[i] = x.splice(0, 64);
      }
      this.archive[data.code_string] = grid;
    },
    CLASS_NAME: 'OpenLayers.Control.PolygonInteraction'
});
