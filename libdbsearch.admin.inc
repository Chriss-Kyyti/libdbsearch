<?php

// $Id$

/**
 * @file
 * Libdbsearch settings form.
 *
 * Last modified: 07.03.2013 Mika Hatakka
 */

/**
 * Set fields for module settings, shown to adminitrator.
 *
 * @return array field description for settings
 */
function libdbsearch_admin() {
  $form = array();
  $form['libdb_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Address of used database'),
    '#default_value' => variable_get('libdb_server', ''),
    '#description' => t("Address where all database querys must send."),
    '#required' => TRUE,
  );

  $form['libdbsearch_extra_info'] = array(
    '#title' => t('Extra information'),
    '#type' => 'textarea',
    '#description' => t('Write here your text to show in at the bottom of block.'),
    '#rows' => 3,
    '#default_value' => variable_get('libdbsearch_extra_info', ''),
  );

  return system_settings_form($form);
}

function libdbsearch_admin_validate($form, &$form_state) {
  if(isset($form_state['values']['libdb_server'])) {
    $srv = $form_state['values']['libdb_server'];
    if( preg_match('/^http:\/\/\s*([^;]*)/mi', $srv) ||
        preg_match('/^https:\/\/\s*([^;]*)/mi', $srv) ) {
      if (substr($srv, -1) != '/') {
        $form_state['values']['libdb_server'] = $srv . '/';
      }
    }
    else {
      form_set_error('URL', t('Give full URL address.'));
    }
  }
}
