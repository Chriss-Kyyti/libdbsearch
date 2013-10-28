<?php

/**
 * @file
 * Libdbsearch settings form.
 *
 * Last modified: 28.10.2013 Mika Hatakka
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

  $form['libdb_ebook_pic'] = array(
      '#type' => 'textfield',
      '#title' => t('EBook link picture'),
      '#default_value' => variable_get('libdb_ebook_pic', ''),
      '#description' => t("Picture for EBook picture link."),
  );

  $form['setup_select_list'] = array(
      '#type' => 'fieldset',
      '#title' => t('Update lists database table'),
      '#description' => t('Dropdown lists in search form are based information in Web-Origo result page. After installation list must be updated manually. In normal use lists are checked by cron every Sunday morning at 3:00 am. If cron is not in use checking must be done also manually.'),
  );

  $form['setup_select_list']['update'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#submit' => array('libdbsearch_list_setup_submit'),
  );

  $form['libdbsearch_search_info'] = array(
      '#title' => t('Search help information'),
      '#type' => 'text_format',
      '#description' => t('Write here your text to show in the libdbsearch page when no keywords are given. Do not change the text format from "Filtered_html". Filtered html is the formatter that is used when rendering text to display.'),
      '#rows' => 3,
      '#default_value' => variable_get('libdbsearch_search_info', 'Give something to search!'),
      '#format' => 'filtered_html',
  );

  $form['libdbsearch_extra_info'] = array(
      '#title' => t('Extra information'),
      '#type' => 'text_format',
      '#description' => t('Write here your text to show in at the bottom of block. Use only "filtered_html" format.'),
      '#rows' => 3,
      '#default_value' => variable_get('libdbsearch_extra_info', ''),
      '#format' => 'filtered_html',
  );

  return system_settings_form($form);
}

function libdbsearch_admin_validate($form, &$form_state) {
  if (isset($form_state['values']['libdb_server'])) {
    $srv = $form_state['values']['libdb_server'];
    if (preg_match('/^http:\/\/\s*([^;]*)/mi', $srv) ||
            preg_match('/^https:\/\/\s*([^;]*)/mi', $srv)) {
      if (substr($srv, -1) != '/') {
        $form_state['values']['libdb_server'] = $srv . '/';
      }
    } else {
      form_set_error('URL', t('Give full URL address.'));
    }
  }
}

/**
 * Submit callback; clear system caches.
 *
 * @ingroup forms
 */
function libdbsearch_list_setup_submit($form, &$form_state) {
  include drupal_get_path('module', 'libdbsearch') . '/includes/libdb_update.inc';
  $origo_upd = new LibdbUpdate();
  $origo_upd->origoUpdateLists();
  drupal_set_message(t('Select lists are updated.'));
}