<?php

/**
 * @file
 * Get connection and retrieve data from remote server.
 *
 * Last modified: 17.10.2013 Mika Hatakka
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

  public function init($type = 'haku', $raw_keys = '') {

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
    
    if(empty($raw_keys)) {
      $raw_keys = $this->linkFromOld();
    }

    $keys = str_replace('%3D', '=', $raw_keys);
    
    if($type == 'haku' && strpos($keys, '=') === FALSE) {
      $keys = 'vapaasana=' . $keys;
    }
    $this->keys = str_replace(array(' ', '#'), array('+', "%23"), $keys);
    
    $this->error = NULL;
  }

  public function askPage() {
    if (empty($this->server)) {
      return 'No Server';
    }
    $data = $this->getMovedPage($this->server, $this->type, $this->keys, $this->cookie);

    if (isset($data['cookie'])) {
      $this->cookie = $data['cookie'];
      $_SESSION['atpconn'] = array(time(), $this->cookie);
    }

    return $data['page'];
  }

  /**
   * Return server address where asked
   * 
   * @return string server address
   */
  public function getServer() {
    return $this->server;
  }
  
  /**
   * Send request to origo database, handle redirection and
   * receives the wanted result page and connectio cookie.
   *
   * @param string $server address of origo database
   * @param string $page name of the page which handle's the query
   * @param string $criteria  query criteria
   * @param string $cookie cookie for current session
   * @return array webpage and cookie in array
   */
  private function getMovedPage($server, $page, $criteria, $cookie = NULL) {
    $webpage = $server . $page . '?' . $criteria;

    if (isset($cookie)) {
      $fp = $this->createHeaderAndGet($cookie, $webpage);
      if ($fp != '' && $fp !== FALSE) { // No moved page
        return array('page' => $fp, 'cookie' => $cookie);
      }
    }
    else {
      $fp = file_get_contents($webpage, FILE_TEXT);
    }
    $homepage = '';
    if (isset($http_response_header)) {
      $findme = "HTTP/1.";
      foreach ($http_response_header as $resp) {
        $pos = strpos($resp, $findme);
        if ($pos !== FALSE) {
          if (strpos($resp, "302") !== FALSE) { // status code 302 found from a different URI
            $findme = "Location:";
          }
          elseif (strpos($resp, "200") !== FALSE) { // OK
            if ($fp === FALSE) {
              return FALSE;
            }
            $homepage = $fp;
            $findme = "Set-Cookie:";
          }
          elseif (($pos = strpos($resp, "Location:")) !== FALSE) {
            $newpage = substr($resp, $pos + 10);
            $findme = "Set-Cookie:";
          }
          elseif (($pos = strpos($resp, "Set-Cookie:")) !== FALSE) {
            $pos = $pos + 12;
            $pos2 = strpos($resp, ";") - $pos;
            $cookie = substr($resp, $pos, $pos2);
            $findme = "äöå";
            if ($homepage != '') { // No moved page
              return array('page' => $homepage, 'cookie' => $cookie);
            }
            break;
          }
        }
        if ($findme == "äöå") {
          break;
        }
      }
      if (isset($newpage)) {
        $homepage = $this->createHeaderAndGet($cookie, $server . "/" . $newpage);
        if ($homepage === FALSE) {
          return FALSE;
        }

        return array('page' => $homepage, 'cookie' => $cookie);
      }
      else {
        return FALSE;
      }
    }
    else {
      return FALSE;
    }
  }

  /**
   * Creates hesder info to search request and
   * sends request and return the result webpage.
   *
   * @param string $cookie current session id
   * @param string $path URL
   * @return string result webpage
   */
  private function createHeaderAndGet($cookie, $path) {
    $opts = array(
      'http' => array(
        'method' => "GET",
        'header' => "Accept-language: en\r\n" .
        "Cookie:" . $cookie
      )
    );
    $context = stream_context_create($opts);
    $homepage = file_get_contents($path, FILE_TEXT, $context);
    return $homepage;
  }

  private function linkFromOld() {
   $result = '';
    if(isset($_GET['teosid'])) {
      $result = 'teosid=' . $_GET['teosid'];
    }
    return $result;
  }
  
}
