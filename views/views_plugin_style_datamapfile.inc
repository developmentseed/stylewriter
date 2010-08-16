<?php
// $Id$

/**
 * @file
 * Extending the view_plugin_style class to provide a mapfile view style.
 */
class views_plugin_style_datamapfile extends views_plugin_style {
    /**
     * Set default options
     */
    function option_definition() {
      $options = parent::option_definition();
      $options['data_view'] = '';
      $options['rules'] = '';
      return $options;
    }

    function data_options() {
      $views = views_get_all_views();
      $options = array();
      foreach ($views as $view) {
        foreach ($view->display as $display => $data) {
          $view->set_display($display);
          if ($data->display_plugin == 'feed') {
            $options[$view->name . ':' . $display] = $view->title . '(' . $view->name . ' - ' . $display . ')';
          }
        }
        $view->destroy();
      }  
      return $options;
    } 

    /**
     * @param $view_display colon-separated view-display
     *
     * return a string representing the URL of a view that returns a Mapnik style
     * with arguments inherited from the current view
     */
    function viewurlargs($view_display) {
      global $base_url;
      list($view_name, $display_name) = explode(':', $view_display);
      if ($view = views_get_view($view_name)) {
        $args = array();
        // Retrieve args/filters from current view
        // to pass on to our vector layer.
        $current_view = views_get_current_view();
        if (isset($current_view)) {
          $args = $current_view->args;
        }
        $view->set_display($display_name);
        return $base_url . '/' . $view->get_url($args);
      }
    }

    /**
     * Accessor for URL
     * @return string $url
     */
    function data_url() {
      return $this->viewurlargs($this->options['data_view']);
    }

    /**
     * Provide a form for setting options.
     *
     * @param array $form
     * @param array $form_state
     */
    function options_form(&$form, &$form_state) { 
      parent::options_form($form, $form_state);

      $form['data_view'] = array(
        '#type' => 'select',
        '#default_value' => $this->options['data_view'],
        '#title' => t('Data View'),
        '#options' => $this->data_options(),
        '#description' => t('Data view (KML)')
      );
      $form['rules'] = array(
        '#type' => 'textarea',
        '#default_value' => $this->options['rules'],
        '#title' => t('Cascadenik'),
        '#description' => t('Cascadenik markup')
      );
    }

    function field_names() {
      return array_keys($this->view->field);
    }
}
