<?php
// $Id$

/**
 * @file Stylewriter layer type definition for OpenLayers
 */
class openlayers_layer_type_stylewriter extends openlayers_layer_type {
  function __construct($layer = array(), $map = array()) {
    parent::__construct($layer, $map);
    if (isset($this->data)) {
      $this->data += $this->options_init();
    }
    else {
      $this->data = $this->options_init();
    }
  }

  /**
   * Provide initial values for options.
   */
  function options_init() {
    return array(
      'serverResolutions' => openlayers_get_resolutions('900913'),
      'maxExtent' => openlayers_get_extent('900913'),
      'projection' => array('900913'),
      'sphericalMercator' => TRUE,
      'osm' => FALSE,
      'baselayer' => FALSE,
      'layer_handler' => 'stylewriter',
      'layer_type' => 'openlayers_layer_type_stylewriter',
      'type' => 'png',
      'buffer' => 0,
      'minzoom' => '0',
    );
  }

  /**
   * Get all views that could be used for data sources
   * This is only a helper for the form below.
   *
   * @return $options array of eligable views
   */
  private function stylewriter_options() {
    $views = views_get_all_views();
    $options = array();
    foreach ($views as $view) {
      foreach ($view->display as $display => $data) {
        $view->set_display($display);
        if (in_array($data->display_plugin, array('mapfile', 'datamapfile'))) {
          $options[$view->name . ':' . $display] = 
            (isset($view->title) ? $view->title : $view->name)
            . ' (' . $view->name . ' - ' . $display . ')';
        }
      }
      $view->destroy();
    }  
    return $options;
  } 

  /**
   * Options form which generates layers
   */
  function options_form() {
    $this->data = isset($this->data) ? $this->data : $this->options_init();

    return array(
      'base_url' => array(
        '#type' => 'textfield',
        '#title' => t('Base URL'),
        '#required' => TRUE,
        '#description' => t('The base URL of the tile server.'),
        '#default_value' => $this->data['base_url']
      ),
      'layer_type' => array(
        '#type' => 'hidden',
        '#value' => 'openlayers_layer_type_stylewriter'
      ),
      'style_view' => array(
        '#type' => 'select',
        '#default_value' => $this->data['style_view'],
        '#title' => t('Style View'),
        '#required' => TRUE,
        '#options' => $this->stylewriter_options(),
        '#description' => t('View with Stylewriter display plugin and style 
          plugin that provides style files')
      ),
      'isBaseLayer' => array(
        '#type' => 'checkbox',
        '#default_value' => $this->data['isBaseLayer'],
        '#title' => t('BaseLayer'),
        '#description' => t('Uncheck to make this map an overlay')
      ),
      'projection' =>
        array(
         '0' => array(
          '#type' => 'hidden',
          '#value' => '900913',
        ),
        '#default_value' => isset($this->data['resolutions']) ? 
          $this->data['resolutions'] : 
          array_map('strval', openlayers_get_resolutions('900913'))
      ),
      'minzoom' => array(
        '#type' => 'textfield',
        '#title' => t('Minimum Zoom Level'),
        '#description' => t('Adjust the farthest that this layer can be zoomed
        out'),
        '#default_value' => $this->data['minzoom'],
      ),
    );
  }

  /**
   * get legend data
   * 
   * @param string $view_display colon-separated view id
   * @return string $themed_legend html for the final legend
   */
  private function get_symbolizer($view_display) {
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $args = array();
      // Retrieve args/filters from current view
      // to pass on to our vector layer.
      $current_view = views_get_current_view();
      if (isset($current_view)) {
        $args = $current_view->args;
        if (isset($current_view->exposed_input)) {
          $view->set_exposed_input($current_view->exposed_input);
        }
      }

      // views sucks
      if ($view->display[$display_name]->display_options['style_options']['colors']['choropleth']['enable']) {
        return 'polygon';
      }
      if ($view->display[$display_name]->display_options['style_options']['scale']['enable']) {
        return 'point';
      }
    }
  }


  /**
   */
  private function get_join_field($view_display) {
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $args = array();
      // Retrieve args/filters from current view
      // to pass on to our vector layer.
      $current_view = views_get_current_view();
      if (isset($current_view)) {
        $args = $current_view->args;
        if (isset($current_view->exposed_input)) {
          $view->set_exposed_input($current_view->exposed_input);
        }
      }
      // views sucks
      return $view->display[$display_name]->display_options['style_options']['fields']['join_field_name'];
    }
  }
  
  /**
   * get legend data
   * 
   * @param string $view_display colon-separated view id
   * @return string $themed_legend html for the final legend
   */
  private function get_value_field($view_display) {
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $args = array();
      // Retrieve args/filters from current view
      // to pass on to our vector layer.
      $current_view = views_get_current_view();
      if (isset($current_view)) {
        $args = $current_view->args;
        if (isset($current_view->exposed_input)) {
          $view->set_exposed_input($current_view->exposed_input);
        }
      }
      // views sucks
      return $view->display[$display_name]->display_options['style_options']['fields']['value'] . '_rendered';
    }
  }

  /**
   * get legend data
   * 
   * @param string $view_display colon-separated view id
   * @return string $themed_legend html for the final legend
   */
  private function get_legend($view_display, $layer_name) {
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $args = array();
      // Retrieve args/filters from current view
      // to pass on to our vector layer.
      $current_view = views_get_current_view();
      if (isset($current_view)) {
        $args = $current_view->args;
        if (isset($current_view->exposed_input)) {
          $view->set_exposed_input($current_view->exposed_input);
        }
      }

      $view->set_items_per_page(1000);
      $view->set_display($display_name);
      $view->set_arguments($args);
      $view->execute();
      if ($view->style_plugin instanceof views_plugin_style_mapfile) {
        $legend_data = $view->style_plugin->legend_data($view->result);
        return ($legend_data['points']) ? $view->style_plugin->theme_legend(
          $legend_data['points'], 
          $legend_data['values'], 
          $layer_name) : '';
      }
    }
  }

  /**
   * get a key map
   *
   * A key map is a Javascript object output with a page that 
   * allows gridded primary keys to be used for polygon interaction.
   * 
   * @param string $view_display colon-separated view id
   * @return array $keymap_data associative array of keys to information
   */
  private function get_keymap($view_display) {
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $args = array();
      // Retrieve args/filters from current view
      // to pass on to our vector layer.
      $current_view = views_get_current_view();
      if (isset($current_view)) {
        /*
        $args = $current_view->args;
        if (isset($current_view->exposed_input)) {
          $view->set_exposed_input($current_view->exposed_input);
        }
         */
      }
      $view->set_items_per_page(1000);
      $view->set_display($display_name);
      $view->execute();
      if ($view->style_plugin instanceof views_plugin_style_mapfile) {
        return $view->style_plugin->keymap_data($view->result);
      }
    }
  }

  /**
   * get join field name
   * 
   * @param string $view_display colon-separated view id
   * @return string $field_name join field name
   */
  public function get_joinfield() {
    list($view_name, $display_name) = explode(':', $this->data['style_view']);
    if ($view = views_get_view($view_name)) {
      $view->set_display($display_name);
      $view->execute();
      if ($view->style_plugin instanceof views_plugin_style_mapfile) {
        return $view->style_plugin->options['fields']['join_field_name'];
      }
    }
  }

  /**
   * @param $view_display colon-separated view-display
   *
   * return a string representing the URL of a view that returns a Mapnik style
   * with arguments inherited from the current view
   */
  private function viewurlargs($view_display) {
    global $base_url;
    list($view_name, $display_name) = explode(':', $view_display);
    if ($view = views_get_view($view_name)) {
      $current_view = views_get_current_view();
      $args = (isset($current_view)) ? $current_view->args : array();
      $view->set_display($display_name);
      return $base_url . '/' . $view->get_url($args);
    }
  }

  /**
   * Render.
   */
  function render(&$map) {
    module_load_include('inc', 'stylewriter', 'stylewriter');

    drupal_add_js(drupal_get_path('module', 'stylewriter') . 
      '/includes/layer_types/js/stylewriter_xyz.js');

    drupal_add_js(drupal_get_path('module', 'stylewriter') . 
      '/includes/layer_types/js/stylewriter.js');

    drupal_add_css(drupal_get_path('module', 'stylewriter') . 
      '/stylewriter.css');

    // avoid overwriting pre-existing options, like a legend key
    $this->data['options'] = array_merge(array(
      'legend' => $this->get_legend($this->data['style_view'], $this->title),
      'symbolizer' => $this->get_symbolizer($this->data['style_view']),
      'join_field' => $this->get_join_field($this->data['style_view']),
      'value_field' => $this->get_value_field($this->data['style_view']),
      'keymap' => $this->get_keymap($this->data['style_view'])
    ), isset($this->data['options']) ? $this->data['options'] : array());

    $this->data['base_url'] = $this->data['base_url'] . '${mapfile}/${z}/${x}/${y}.${format}';
    $this->data['mapfile'] = $this->viewurlargs($this->data['style_view']);

    return $this->options;
  }
}
