<?php
// $Id$

/**
 * @file stylewriter styles
 */

function template_preprocess_stylewriter_mapfile(&$vars) {
  $view = $vars['view'];
  $points = $view->style_plugin->map_rows($vars['rows']);

  $rows = '';
  foreach ($points as $point) {

    $rows .= theme('stylewriter_rule',
      $point['join_field_name'],
      $point['join_field_value'],
      $point['polygon'],
      $view->style_plugin->options['colors']['stroke'],
      $view->style_plugin->options['metawriter'],
      $point['point']
    );
  }
  if ($view->style_plugin->options['fields']['data_type'] == 'shape') {
    exit(theme('stylewriter_document', 
      $rows, 
      $view->style_plugin->options['fields']['data_url'],
      $view->style_plugin->options['fields']['data_type'],
      // layer: OGRGeoJSON
      array(
        'file' => '[tile_dir]/[z]/[x]/[y].json',
        'type' => 'json',
        'name' => 'meta1',
      )
    ));
  }
  else {
    exit(theme('stylewriter_document', 
      $rows, 
      $view->style_plugin->data_url(),
      $view->style_plugin->options['fields']['data_type'],
      // layer: OGRGeoJSON
      array(
        'file' => '[tile_dir]/[z]/[x]/[y].json',
        'type' => 'json',
        'name' => 'meta1',
      ),
      'OGRGeoJSON'
    ));
  }
}

function template_preprocess_stylewriter_datamapfile(&$vars) {
  $view = $vars['view'];
  exit(theme('stylewriter_document', 
    $view->style_plugin->options['rules'], 
    $view->style_plugin->data_url(),
    'ogr'));
}
