// $Id: openlayers_behavior_zoomtolayer.js,v 1.1.2.7 2010/05/22 22:36:45 zzolo Exp $

/**
 * OpenLayers Zoom to Layer Behavior
 */
Drupal.behaviors.stylewriter_pointhover = function(context) {
  var layerextent, layers, data = $(context).data('openlayers');
  if (data && data.map.behaviors['stylewriter_pointhover']) {
    map = data.openlayers;
    layers = map.getLayersBy('drupalID', 
      data.map.behaviors['stylewriter_pointhover'].layer);
    console.log(layers);
  }
}
