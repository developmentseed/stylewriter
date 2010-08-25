// $Id$

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
OpenLayers.Control.GridHover = OpenLayers.Class(OpenLayers.Control, {

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
    clickCallback: "click",

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
    handler: null,
    
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
     * Constructor: <OpenLayers.Control.GridHover>
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

        this.handler = new OpenLayers.Handler.Hover(
          this, {
              'move': this.cancelHover,
              'pause': this.getInfoForHover
          },
          OpenLayers.Util.extend(this.handlerOptions.hover || {}, {
              'delay': 30
            }
          )
        );
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
          // console.log('offsetting');
          grid = this.archive[$(this.target).attr('src')]
          if (grid === true) { // is downloading
            console.log('downloading');
            return;
          }
          offset = [
                Math.floor((evt.pageX - $(evt.target).offset().left) / 4),
                Math.floor((evt.pageY - $(evt.target).offset().top) / 4)];
          if(grid[offset[1]][offset[0]]) {
            key = grid[offset[1]][offset[0]];
            if (key !== this.key) {
              this.callbacks['out'](this.layer.options.keymap[this.key], this.layer);
            }
            // save this key so that we know whether the next access is to this
            this.key = key;
            if (this.layer.options.keymap[key]) {
              this.callbacks['over'](this.layer.options.keymap[this.key], this.layer);
            }
          }
          else {
            this.callbacks['out'](this.layer.options.keymap[this.key], this.layer);
          }
        }
        else {
          this.callbacks['out']({}, this.layer);
          if (!this.archive[$(evt.target).attr('src')]) {
            this.target.req = true;
            try {
              this.archive[$(evt.target).attr('src')] = true;
              this.target.hoverRequest = $.ajax(
                {
                  'url': $(evt.target).attr('src').replace('png', 'grid.json'), 
                  context: this,
                  success: $.proxy(this.readDone, this),
                  error: function() {},
                  dataType: 'jsonp',
                  jsonpCallback: "f" +this.encode_base64($.map($(evt.target).attr('src')))
                }
              );
            } catch(err) {
              this.archive[$(evt.target).attr('src')] = false;
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
      this.archive[$(this.target).attr('src')] = grid;
    },
    CLASS_NAME: "OpenLayers.Control.GridHover"
});

Drupal.behaviors.stylewriter_gridhover = function(context) {
  var layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_gridhover']) {
    map = data.openlayers;
    layer = map.getLayersBy('drupalID', 
      data.map.behaviors['stylewriter_gridhover'].layer)[0];
    h = new OpenLayers.Control.GridHover({
      layer: layer,
      callbacks: {
        'over': Drupal.StyleWriterTooltips.select,
        'out': Drupal.StyleWriterTooltips.unselect
      }
    });
    map.addControl(h);
    h.activate();
  }
};

Drupal.StyleWriterTooltips = {};

Drupal.StyleWriterTooltips.click = function(feature) {
  var html = '';
  if (feature.attributes.name) {
    html += feature.attributes.name;
  }
  if (feature.attributes.description) {
    html += feature.attributes.description;
  }
  var link;
  if ($(html).is('a')) {
    link = $(html);
  }
  else if ($(html).children('a').size() > 0) {
    link = $(html).children('a')[0];
  }
  if (link) {
    var href = $(link).attr('href');
    if (Drupal.OpenLayersPermalink && Drupal.OpenLayersPermalink.addQuery) {
      href = Drupal.OpenLayersPermalink.addQuery(href);
    }
    window.location = href;
    return false;
  }
  return;
};

Drupal.StyleWriterTooltips.getToolTip = function(feature) {
  var text = "<div class='openlayers-tooltip'>";
  if (feature.name) {
    text += "<div class='openlayers-tooltip-name'>" + feature.name + "</div>";
  }
  if (feature.description) {
    text += "<div class='openlayers-tooltip-description'>" + feature.description + "</div>";
  }
  text += "</div>";
  return $(text);
};

Drupal.StyleWriterTooltips.select = function(feature, layer) {
  var tooltip = Drupal.StyleWriterTooltips.getToolTip(feature);
  $(layer.map.div).css('cursor', 'pointer');
  $(layer.map.div).append(tooltip);
};

Drupal.StyleWriterTooltips.unselect = function(feature) {
  $(layer.map.div).css('cursor', 'default');
  $(layer.map.div).children('div.openlayers-tooltip').fadeOut('fast', function() { $(this).remove(); });
};
