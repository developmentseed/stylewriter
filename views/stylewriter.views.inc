<?php
// $Id$

/**
 * @file
 * This file holds main views functions for the stylewriter module.
 *
 * @ingroup stylewriter 
 */

/**
 * Implementation of hook_views_plugins().
 */
function stylewriter_views_plugins() {
  return array(
    'module' => 'stylewriter',
    'display' => array(
      'datamapfile' => array(
        'title' => t('Data Mapfile'),
        'help' => t('A Mapfile display'),
        'handler' => 'stylewriter_plugin_display_datamapfile',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses hook menu' => TRUE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'accept attachments' => FALSE,
        'admin' => t('Stylewriter Data Mapfile'),
        'type' => 'feed',
        'parent' => 'feed'
      ),
      'mapfile' => array(
        'title' => t('Mapfile'),
        'help' => t('A Mapfile display'),
        'handler' => 'stylewriter_plugin_display_mapfile',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses hook menu' => TRUE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'accept attachments' => FALSE,
        'admin' => t('Stylewriter Mapfile'),
        'type' => 'feed',
        'parent' => 'feed'
      ),
    ),
    'style' => array(
      'mapfile' => array(
        'title' => t('Mapfile'),
        'help' => t(''),
        'handler' => 'views_plugin_style_mapfile',
        'theme' => 'stylewriter_mapfile',
        'theme file' => 'stylewriter.theme.inc',
        'theme path' => drupal_get_path('module', 'stylewriter') .'/views',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses fields' => TRUE,
        'uses row plugin' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'feed',
        'even empty' => TRUE,
      ),
      'datamapfile' => array(
        'title' => t('Data Mapfile'),
        'help' => t(''),
        'handler' => 'views_plugin_style_datamapfile',
        'theme' => 'stylewriter_datamapfile',
        'theme file' => 'stylewriter.theme.inc',
        'theme path' => drupal_get_path('module', 'stylewriter') .'/views',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses fields' => TRUE,
        'uses row plugin' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'feed',
        'even empty' => TRUE,
      ),
      'legend' => array(
        'title' => t('Map Legend'),
        'help' => t(''),
        'handler' => 'views_plugin_style_maplegend',
        'theme' => 'stylewriter_maplegend',
        'theme path' => drupal_get_path('module', 'stylewriter') .'/views',
        'path' => drupal_get_path('module', 'stylewriter') .'/views',
        'uses fields' => TRUE,
        'uses row plugin' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'normal',
        'even empty' => TRUE,
      ),
    ),
  );
}

