// $Id$

/**
 * Openlayer layer handler for Stylewriter layer
 */
Drupal.openlayers.layer.stylewriter = function (title, map, options) {
  options.projection = 'EPSG:'+options.projection;
    if (options.maxExtent !== undefined) {
      options.maxExtent = new OpenLayers.Bounds.fromArray(options.maxExtent) || 
      new OpenLayers.Bounds(-20037508.34, -20037508.34, 20037508.34, 20037508.34);
  }

  var style_url = options.base_url;
  
  var layer = new OpenLayers.Layer.StyleWriter(
    title,
    style_url, 
    options);

  layer.styleMap = Drupal.openlayers.getStyleMap(map, options.drupalID);
  return layer;
};
