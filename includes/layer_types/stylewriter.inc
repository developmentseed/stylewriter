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
      'osm' => FALSE,
      'baselayer' => FALSE,
      'layer_handler' => 'stylewriter',
      'layer_type' => 'openlayers_layer_type_stylewriter',
      'type' => 'png',
    );
  }

  function stylewriter_options() {
    $views = views_get_all_views();
    $options = array();
    foreach ($views as $view) {
      foreach ($view->display as $display => $data) {
        $view->set_display($display);
        if ($data->display_plugin == 'mapfile') {
          // TODO: probably not best to count on no colon
          $options[$view->name . ':' . $display] = $view->title . '(' . $view->name . ' - ' . $display . ')';
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
    if (!$this->data) {
      $this->data = $this->options_init();
    }
    return array(
      'base_url' => array(
        '#type' => 'textfield',
        '#title' => t('Base URL'),
        '#description' => t('The base URL of the tile server.'),
        '#default_value' => $this->data['base_url']
      ),
      'layer_type' => array(
        '#type' => 'hidden',
        '#value' => 'openlayers_layer_type_stylewriter'
      ),
      'data_url' => array(
        '#type' => 'textfield',
        '#default_value' => $this->data['data_url'],
        '#title' => t('Data URL'),
        '#description' => t('URL to the zipped shapefile or other 
          vector data source')
      ),
      'style_view' => array(
        '#type' => 'select',
        '#default_value' => $this->data['style_view'],
        '#title' => t('Style View'),
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
      ),
      '#default_value' => ($this->data['resolutions']) ? 
        $this->data['resolutions'] : 
        array_map('strval', openlayers_get_resolutions('900913'))
    );
  }

  function get_legend($view_display) {
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

      $args['legend'] = TRUE; // trigger plugin rendering to return legend
      $features = $view->execute_display($display_name, $args);
      $view->destroy();

      return $features;
    }
  }

  /**
   * @param $view_display colon-separated view-display
   *
   * return a string representing the URL of a view that returns a Mapnik style
   * with arguments inherited from the current view
   */
  function viewurlargs($view_display) {
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

      return $features;
    }
  }

  /**
   * Render.
   */
  function render(&$map) {
    module_load_include('inc', 'stylewriter', 'stylewriter');
    drupal_add_js(drupal_get_path('module',  'stylewriter') .'/includes/layer_types/stylewriter.js');
    drupal_add_css(drupal_get_path('module', 'stylewriter') .'/stylewriter.css');
    $this->data['options'] = array();
    $this->data['options']['legend'] = $this->get_legend($this->data['style_view']);
    $this->data['base_url'] = 
      $this->data['base_url'] . 
      stylewriter_base64en($this->data['data_url']) . '/' .
      stylewriter_base64en($this->viewurlargs($this->data['style_url'])) . '/' .
      '${z}/${x}/${y}.png';
    return $this->options;
  }
}
