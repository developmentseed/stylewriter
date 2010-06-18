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
    return '<pre>' . check_plain($this->view->render()) . '</pre>';
  }

  /**
   * Instead of going through the standard views_view.tpl.php, delegate this
   * to the style handler.
   */
  function render() {
    if (isset($this->view->args['legend'])) {
      $points = $this->view->style_plugin->legend_data($this->view->result);
      return $this->view->style_plugin->theme_legend($points);
    }
    else {
      return $this->view->style_plugin->render($this->view->result);
    }
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
