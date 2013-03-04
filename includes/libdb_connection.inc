<?php

/**
 * @file
 * Get connection and retrieve data from remote server.
 *
 * Last modified: 04.03.2013 Mika Hatakka
 */

/**
 * 
 */
class LibdbConnection {

  private $server;
  private $cookie;
  private $type;
  private $keys;
  private $error;

  public function init($type = 'haku', $keys = '') {

    $this->server = variable_get('libdb_server', '');

    $this->cookie = NULL;
    if (isset($_SESSION['atpconn'])) {
      if (($_SESSION['atpconn'][0] + (900)) > time()) {
        $this->cookie = $_SESSION['atpconn'][1];
      }
      else {
        unset($_SESSION['atpconn']);
      }
    }

    $this->type = $type . '.asp';

    $this->keys = '?' . str_replace(array(' ', '#'), array('+', "%23"), $keys);
    $this->error = NULL;
  }

  function getPage() {
    if (empty($this->server)) {
      return 'No Server';
    }
    $page = 'No Data';

    
    
    
    

    if (isset($this->cookie)) {
      $_SESSION['atpconn'] = array(time(), $this->cookie);
    }
    return $page;
  }

  private function askPage() {

    return 'No socket';
  }

  function extractHeader(&$homepage) {
    $header = '';
    $res = explode('<!DOCTYPE', $homepage);
    if (count($res) == 1) {
      $res = explode('<html', $homepage);
      if (count($res) == 1) {
        $res = explode('<head', $homepage);
        if (count($res) != 1) {
          $homepage = '<head' . $res[1];
          $header = $res[0];
        }
      }
      else {
        $homepage = '<html' . $res[1];
        $header = $res[0];
      }
    }
    else {
      $homepage = '<!DOCTYPE' . $res[1];
      $header = $res[0];
    }
    return $header;
  }

}
