<?php

/**
 * @file
 * Page creation function for library database search.
 *
 * Last modified: 06.03.2013 Mika Hatakka
 */

/**
 * Pafe for search result list.
 * 
 * @param type $data
 * @return type
 */
function libdbsearch_set_haku_data($data) {
  $output = '';
  if (isset($data['Hakuinfo']['Teksti'])) {
    foreach ($data['Hakuinfo']['Teksti'] as $val) {
      $output .= check_plain($val) . '<br />';
    }
  }
  
  $info = _libdbsearch_add_info($data);
  if(count($info) > 0) {
    $output .= $info[0];
  }

  $navigation = _libdbsearch_add_navi($data);
  if(count($navigation) > 0) {
    $output .= $navigation[0];
  }
  
  $header = array();
  $rows = array();
  if (isset($data['Hakutulos'])) {
    if (isset($data['Hakutulos']['Selausotsikot']['Totsikko'])) {
      foreach ($data['Hakutulos']['Selausotsikot']['Totsikko'] as $value) {
        $header[] = _libdbsearch_extract_text($value);
      }
    }
    if (isset($data['Hakutulos']['Selauslista']['Teos'])) {
      foreach ($data['Hakutulos']['Selauslista']['Teos'] as $value) {
        $row = array();
        foreach ($value as $key => $val) {
          $row[] = _libdbsearch_extract_value($val);
        }
        $rows[] = $row;
      }
    }
  }

  $output .= theme('table', array('header' => $header, 'rows' => $rows));

  if(count($info) > 1) {
    $output .= $info[1];
  }

  if(count($navigation) > 1) {
    $output .= $navigation[1];
  }

  return $output;
}

/**
 * Prints out text or link from section.
 * 
 * @param type $value
 * @return type
 */
function _libdbsearch_extract_text($value) {
  if (isset($value['Teksti'])) {
    return $value['Teksti'];
  }
  elseif (isset($value['Linkki'])) {
    $linkki = str_replace('.asp?', '/', $value['Linkki']['Losoite']);
    return l($value['Linkki']['Lteksti'], 'libdbsearch/' . $linkki);
  }
}

/**
 * Prints out text or link from two level section.
 * 
 * @param type $value
 * @return type
 */
function _libdbsearch_extract_value($value) {
  if (is_array($value)) {
    return _libdbsearch_extract_text($value);
  }
  else {
    return $value;
  }
}

/**
 * Create's page navigation .
 * 
 * @param type $data
 * @return type
 */
function _libdbsearch_add_navi($data) {
  $output = array();;
  if (isset($data['Navigointi'])) {
    foreach ($data['Navigointi'] as $val) {
      $out_navi = '';
      if (isset($val['Teksti'])) {
        $out_navi .= $val['Teksti'];
      }
      if (isset($val['Valinta'])) {
        foreach ($val['Valinta'] as $value) {
          $out_navi .= _libdbsearch_extract_navi_text($value);
        }
      }
      $output[] = $out_navi;
    }
  }
  return $output;
}

/**
 * Extracts one navigation component.
 * 
 * @param type $data
 * @return string
 */
function _libdbsearch_extract_navi_text($data) {
  $output = '';
  if (is_array($data)) {
    if(isset($data['@attributes']['separator'])) {
      if($data['@attributes']['separator'] == 'yes') {
        $output .= ' | ';
      }
    }
    if(isset($data['Haettu'])) {
      $output .= $data['Haettu'] . ' ';
    }
    if(isset($data['Linkki'])) {
      $output .= _libdbsearch_extract_text($data) . ' ';
    }
    
  }
  else {
    $output .= $data . ' ';
  }
  return $output;
}

/**
 * Create's section info data
 * 
 * @param type $data
 * @return string
 */
function _libdbsearch_add_info($data) {
  $output = array();;
  if (isset($data['Lohko'])) {
    foreach ($data['Lohko'] as $val) {
      $out_info = '';
      if (isset($val['Teksti'])) {
        $out_info .= _libdbsearch_extract_text($val);
      }
      if(!empty($out_info)) {
        $out_info .= '<br />';
      }
      $output[] = $out_info;
    }
  }
  return $output;
}
