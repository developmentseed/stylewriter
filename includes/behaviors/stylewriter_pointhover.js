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
 * Inherits from:
 *  - <OpenLayers.Control>
 */
OpenLayers.Control.PointHover = OpenLayers.Class(OpenLayers.Control, {

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
          if (grid === true || grid == undefined) { // is downloading
            console.log('downloading');
            return;
          }
          offset = [
                Math.floor((evt.pageX - $(evt.target).offset().left) / 4),
                Math.floor((evt.pageY - $(evt.target).offset().top) / 4)];
          lonLat = this.map.getLonLatFromPixel(evt.xy);
          for(var i = 0; i < grid.length; i++) {
            if(grid[i].geometry.intersects(new OpenLayers.Geometry.Point(lonLat.x, lonLat.y))) {
              console.log(grid[i]);
            }
          }
          if(true) {
            /*
            key = grid[offset[1]][offset[0]];
            if (key !== this.key) {
              this.callbacks['out'](this.layer.options.keymap[this.key], this.layer);
            }
            // save this key so that we know whether the next access is to this
            this.key = key;
            if (this.layer.options.keymap[key]) {
              this.callbacks['over'](this.layer.options.keymap[this.key], this.layer);
            }
            */
          }
          else {
            this.callbacks['out'](this.layer.options.keymap[this.key], this.layer);
          }
        }
        else {
          this.callbacks['out']({}, this.layer);
          console.log('dling');
          if (!this.archive[$(evt.target).attr('src')]) {
            this.target.req = true;
            try {
              this.archive[$(evt.target).attr('src')] = true;
              this.target.hoverRequest = $.ajax(
                {
                  'url': $(evt.target).attr('src').replace('png', 'json'), 
                  context: this,
                  success: $.proxy(this.readDone, this),
                  error: function() {},
                  dataType: 'jsonp',
                  jsonpCallback: "f" + $.map($(evt.target).attr('src').split('/'), parseInt).slice(-3).join('x')
                }
              );
            } catch(err) {
              this.archive[$(evt.target).attr('src')] = false;
            }
          }
        }
    },

    readDone: function(data) {
      this.archive[$(this.target).attr('src')] = this.format.read(data);
    },
    CLASS_NAME: "OpenLayers.Control.PointHover"
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
    h = new OpenLayers.Control.PointHover({
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
  // @TODO: Make this a behavior option and allow interaction with other
  // behaviors like the MN story popup.
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
