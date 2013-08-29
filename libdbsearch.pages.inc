<?php

/**
 * @file
 * Page creation function for library database search.
 *
 * Last modified: 30.06.2013 Mika Hatakka
 */

/**
 * Process variables for libdbsearch-results.tpl.php.
 *
 * @see libdbsearch-results.tpl.php
 */
function template_preprocess_libdbsearch_results(&$variables) {
  $variables['libdb_top_info'] = '';
  $variables['libdb_top_pager'] = '';
  $variables['libdb_extra_info'] = '';
  $variables['libdb_bottom_info'] = '';
  $variables['libdb_bottom_pager'] = '';
  $result = $variables['SResult'];
  $action = $variables['SAction'];

  switch ($action) {
    case 'teos' :
      $variables['libdb_result'] = theme('libdbsearch_work', $result);
      break;
    default :
      $variables['libdb_result'] = theme('libdbsearch_result', $result['Hakutulos']);
      break;
  }

  // Information about search reasult, if ok and result counts.
  if (isset($result['Hakuinfo']['Teksti'])) {
    if (is_array($result['Hakuinfo']['Teksti'])) {
      foreach ($result['Hakuinfo']['Teksti'] as $val) {
        $variables['libdb_top_info'] .= _libdbsearch_t($val) . '<br />';
      }
    } else {
      $variables['libdb_top_info'] .= _libdbsearch_t($result['Hakuinfo']['Teksti']);
    }
  }

  // Navigation to top of the result area.
  $navigation = _libdbsearch_add_navi($result);
  if (count($navigation) > 0) {
    if (preg_match("/<a/", $navigation[0]) == 0) {
      $variables['libdb_bottom_pager'] = '';
    } else {
      $variables['libdb_top_pager'] .= '<div class="item-list">';
      $variables['libdb_top_pager'] .= $navigation[0];
      $variables['libdb_top_pager'] .= '</div>';
    }
  }
  if (count($navigation) > 1) {
    if (preg_match("/<a/", $navigation[1]) == 0) {
      $variables['libdb_bottom_pager'] = '';
    } else {
      $variables['libdb_bottom_pager'] .= '<div class="item-list">';
      $variables['libdb_bottom_pager'] .= $navigation[1];
      $variables['libdb_bottom_pager'] .= '</div>';
    }
  }
}

/**
 * Process variables for libdbsearch-result.tpl.php.
 *
 * @see libdbsearch-result.tpl.php
 */
function template_preprocess_libdbsearch_result(&$variables) {
  $variables['result_rows'] = '';
  if (isset($variables['Selausotsikot']['Totsikko'])) {
    foreach ($variables['Selausotsikot']['Totsikko'] as $key => $value) {
      $variables['theader_' . ($key)] = _libdbsearch_extract_text($value);
    }
    $my_teos = _libdbsearch_make_array($variables['Selauslista']['Teos']);
    foreach ($my_teos as $key => $value) {
      $variables['result_rows'] .= theme('libdbsearch_result_row', array('libdb_data' => $value));
    }
  } elseif (isset($variables['Aineistolajittain']['Aineistolaji'])) {
    $hdr = FALSE;
    $my_arr = _libdbsearch_make_array($variables['Aineistolajittain']['Aineistolaji']);
    foreach ($my_arr as $value) {
      $row_cnt = 0;
      foreach ($value as $key => $val) {
        if ($hdr == FALSE) {
          $variables['theader_' . $row_cnt] = _libdbsearch_t($key);
          $row_cnt++;
        }
        $variables['row_data_' . $row_cnt] = _libdbsearch_extract_value($val);
      }
      $variables['result_rows'] .= theme('libdbsearch_result_row', array('libdb_data' => $value));
      $hdr = TRUE;
    }
  }
}

/**
 * Process variables for libdbsearch-result-row.tpl.php.
 *
 * @see libdbsearch-result-row.tpl.php
 */
function template_preprocess_libdbsearch_result_row(&$variables) {
  $data_cnt = 0;
  foreach ($variables['libdb_data'] as $row) {
    $variables['row_data_' . $data_cnt] = _libdbsearch_extract_value($row);
    $data_cnt++;
  }
}

/**
 * Process variables for libdbsearch-avail.tpl.php.
 *
 * @see libdbsearch-avail.tpl.php
 */
function template_preprocess_libdbsearch_avail(&$variables) {
  $variables['avail_rows'] = '';
  $hdr = TRUE;
  $result = $variables['SResult'];
  $variables['subheader'] = _libdbsearch_t($result['Valiotsikko']);
  $my_arr = _libdbsearch_make_array($result['Kirjasto']);
  foreach ($my_arr as $key => $value) {
    $value['Sijainti'] = _libdbsearch_make_array($value['Sijainti']);
    if ($hdr == TRUE) {
      $headers = array_keys($value['Sijainti'][0]);
      $variables['theader_0'] = t('Location', array(), array('context' => 'libdb'));
      foreach ($headers as $key => $val) {
        if($key == 0) {
          continue;
        }
        $variables['theader_' . ($key)] = _libdbsearch_t($val);
      }
    }
    $variables['avail_rows'] .= theme('libdbsearch_avail_row', $value);
    $hdr = FALSE;
  }
}

/**
 * Process variables for libdbsearch-avail-row.tpl.php.
 *
 * @see libdbsearch-avail-row.tpl.php
 */
function template_preprocess_libdbsearch_avail_row(&$variables) {
  $variables['kirjastonimi'] = $variables['Kirjastonimi'];
  foreach ($variables['Sijainti'] as $row=>$loc) {
    foreach ($loc as $key => $value) {
      if (is_array($value)) {
        $value = current($value);
      }
      $variables[strtolower($key)][$row] = $value;
    }
  }
}

/**
 * Process variables for libdbsearch-avail-libs.tpl.php.
 *
 * @see libdbsearch-avail-libs.tpl.php
 */
function template_preprocess_libdbsearch_avail_libs(&$variables) {
  $result = $variables['SResult'];
  $variables['avail_libs_rows'] = '';
  $hdr = FALSE;
  if (isset($result['Valiotsikko'])) {
    $variables['subheader'] = _libdbsearch_t($result['Valiotsikko']);
  }
  if (isset($result['Teksti'])) {
    $variables['subtext'] = _libdbsearch_t($result['Teksti']);
  }
  $my_arr = _libdbsearch_make_array($result['Teos']);
  foreach ($my_arr as $key => $value) {
    if ($hdr == FALSE) {
      $headers = array_keys($value);
      foreach ($headers as $key => $val) {
        $variables['theader_' . ($key)] = _libdbsearch_t($val);
      }
    }
    $variables['avail_libs_rows'] .= theme('libdbsearch_avail_libs_row', array('work' => $value));
    $hdr = TRUE;
  }
}

/**
 * Process variables for libdbsearch-avail-libs-row.tpl.php.
 *
 * @see libdbsearch-avail-libs-row.tpl.php
 */
function template_preprocess_libdbsearch_avail_libs_row(&$variables) {
  foreach ($variables['work'] as $key => $value) {
    if (is_array($value)) {
      $variables[strtolower($key)] = _libdbsearch_extract_text($value);
    } else {
      $variables[strtolower($key)] = $value;
    }
  }
}

/**
 * Process variables for libdbsearch-work.tpl.php.
 *
 * @see libdbsearch-work.tpl.php
 */
function template_preprocess_libdbsearch_work(&$variables) {
  if (isset($variables['Lohko'])) {
    $my_arr = _libdbsearch_make_array($variables['Lohko']);
    $links = _libdbsearch_add_info(array('Lohko' => $my_arr));

    if (isset($links['Varaa teos'])) {
      $variables['work_reserve'] = $links['Varaa teos'];
    }
    if (isset($links['Tarkat tiedot'])) {
      $variables['work_all_info'] = $links['Tarkat tiedot'];
    }
    if (isset($links['Saatavuustiedot'])) {
      $variables['work_avail_info'] = $links['Saatavuustiedot'];
    }
    if (isset($links['Näytä sisältö / kappaleet'])) {
      $variables['work_content'] = $links['Näytä sisältö / kappaleet'];
    }
    if (isset($links['Lainaa sähköinen kirja'])) {
      $variables['work_elink'] = _libdbsearch_set_elink($links['Lainaa sähköinen kirja']);
    }
    if (isset($links['Esittelyteksti'])) {
      $variables['work_introduction'] = $links['Esittelyteksti'];
    }
  }

  $work_arr = _libdbsearch_make_array($variables['Teostiedot']['Rivi']);
  $variables['rows_data'] = '';
  foreach ($work_arr as $value) {
    $variables['rows_data'] .= theme('libdbsearch_work_row', $value);
  }

  if (isset($variables['Sisaltyy_teokseen']['Rivi'])) {
    $variables['part_header'] = t('Part of', array(), array('context' => 'libdb'));
    $work_arr = _libdbsearch_make_array($variables['Sisaltyy_teokseen']['Rivi']);
    $variables['part_data'] = '';
    foreach ($work_arr as $value) {
      $variables['part_data'] .= theme('libdbsearch_work_row', $value);
    }
  }

  if (isset($variables['Kuva'])) {
    $image = array(
        'path' => $variables['Kuva'],
        'width' => 100,
//      'height' => 50,
        'alt' => 'Teoksen kansikuva',
//      'title' => '',
        'attributes' => array('class' => 'work_pic'),
    );
    $variables['work_image'] = theme('image', $image);
  }

//  if (isset($variables['Esittelyteksti'])) {
//    $variables['work_introduction'] = $variables['Esittelyteksti'];
//  }

  if (isset($variables['Teostiedot']['Rivi'])) {
    $variables['work_tip_link'] = _libdbsearch_tip_link($variables['Teostiedot']['Rivi']);
  }


  if (isset($variables['Saatavuus'])) {
    $variables['all_avail'] = theme('libdbsearch_avail_libs', array('SResult' => $variables['Saatavuus']));
  }

  if (isset($variables['Valitun_saatavuus'])) {
    $variables['lib_avail'] = theme('libdbsearch_avail', array('SResult' => $variables['Valitun_saatavuus']));
  }
}

/**
 * Process variables for libdbsearch-work-row.tpl.php.
 *
 * @see libdbsearch-work-row.tpl.php
 */
function template_preprocess_libdbsearch_work_row(&$variables) {
  $variables['theader'] = isset($variables['Selite']) ? _libdbsearch_t($variables['Selite']) : '';
  if (isset($variables['Tieto'][0])) {
    $str = '';
    foreach ($variables['Tieto'] as $val) {
      $str .= _libdbsearch_extract_value($val) . '<br />';
    }
  } else {
    $str = isset($variables['Tieto']) ? _libdbsearch_extract_value($variables['Tieto']) : '';
  }
  $variables['tdata'] = $str;
}
