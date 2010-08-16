// $Id$

/**
 * @file
 * Layer handler for Stylewriter layers
 */

/**
 * Openlayer layer handler for Stylewriter layer
 */
Drupal.openlayers.layer.stylewriter = function (title, map, options) {
  var styleMap = Drupal.openlayers.getStyleMap(map, options.drupalID);

  if (options.maxExtent !== undefined) {
    options.maxExtent = new OpenLayers.Bounds.fromArray(options.maxExtent) || new OpenLayers.Bounds(-20037508.34, -20037508.34, 20037508.34, 20037508.34);
  }
  options.projection = 'EPSG:'+options.projection;
  options.sphericalMercator = true;
  options.buffer = 0;
  
  var layer = new OpenLayers.Layer.StyleWriter(title, options.base_url, options);
  layer.styleMap = styleMap;
  return layer;
};
