<?php
// $Id$

/**
 * @file
 * Display plugin for Mapfile
 *
 * @ingroup openlayers
 */

class stylewriter_plugin_display_mapfile extends views_plugin_display_feed {
  function uses_breadcrumb() { return FALSE; }
  function get_style_type() { return 'mapfile'; }

  function execute() {
    return $this->view->render();
  }

  function preview() {
    return '<pre>This views type does not support previewing.</pre>';
  }

  function render() {
    return $this->view->style_plugin->render($this->view->result);
  }

  function option_definition() {
    $options = parent::option_definition();

    // Prevent the title from being translated at the view level
    // so that it can be translated at the OpenLayers level
    $options['title']['translatable'] = 0;
    $options['displays'] = array('default' => array());

    // Overrides for standard stuff:
    $options['style_plugin']['default'] = 'mapfile';
    $options['row_plugin']['default'] = '';
    $options['defaults']['default']['row_plugin'] = FALSE;
    $options['defaults']['default']['row_options'] = FALSE;
    return $options;
  }

  function options_summary(&$categories, &$options) {
    parent::options_summary($categories, $options);
    unset($options['header']);
    unset($options['footer']);
    unset($options['empty']);
    unset($options['exposed_block']);
    unset($options['analyze-theme']);
  }
}
