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
      $point['color'],
      $view->style_plugin->options['colors']['opacity']);
  }
  $vars['rows'] = $rows;
  $vars['viewtitle'] = $view->name;
  exit(theme('stylewriter_document', $rows));
}
