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
  private $page;
  private $header;
  private $error;
  private $clear_session;

//  public function __construct() {
//    $this->server = NULL;
//    $this->cookie = NULL;
//    $this->type = NULL;
//    $this->keys = NULL;
//    $this->page = 'No data';
//    $this->header = '';
//  }

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

    $this->type = $type;
    $this->clear_session = FALSE;
    
    $my_keys = explode('&', $keys);
    if($my_keys !== FALSE && count($my_keys) > 1) {
      $keys = $my_keys[0];
      if($my_keys[1] == 'sess=new') {
        $this->clear_session = TRUE;
        $this->cookie = NULL;
      }
    }
    
    $this->keys = str_replace(' ', '+', $keys);
    $this->error = NULL;
    $this->page = 'No data';
    $this->header = '';
  }

  public function askPage() {
    if (empty($this->server)) {
      return 'No Server';
    }
    $result = '';
    $request_url = $this->server . $this->type . ".asp?vapaasana=" . $this->keys;
    
    dpm($request_url);
    dpm($this->cookie, 'Cookie before');
    
    $ch = curl_init();
    
    $response = $this->getPage($ch, $request_url, $this->cookie);
    
    dpm($response, 'Response');
    
    $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    
    
    // Kaksi väylää
    // 
    // cookie == NULL
    //   täytyy seurata manuaalisesti
    //   tarvitaan header length
    //   
    //   
    //   
    // cookie != NULL
    //   response on tulos
    //   cookie aikoineen muistiin
    //
    
    
    
    
    

    if ($http_code == 200) {
      
      if($this->cookie === NULL) { // New session started
        $header_length = curl_getinfo($ch, CURLINFO_HEADER_SIZE);
        $new_url = curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
        
        $this->header = substr($response, 0, $header_length);
        $m = '';
        preg_match_all('/^Set-Cookie:\s*([^;]*)/mi', $this->header, $m);
        if (!empty($m[1])) {
          $this->cookie = $m[1][count($m[1]) - 1];
        }
        
        dpm($m);
        dpm($this->cookie, 'Cookie Before Second Hit');
        
        $new_ch = curl_copy_handle($ch);
        $new_resp = $this->getPage($new_ch, $new_url, $this->cookie);
        
        dpm($new_resp, 'New response');
        
        if(curl_getinfo($new_ch, CURLINFO_HTTP_CODE) == 200) {
          $this->page = $new_resp;
        }
        else {
          $this->error = curl_error($new_ch);
          $this->page = '';
        }
        
      }
      else {
        $this->page = $response;
      }
      
      
      
      
    }
    else {
      $this->error = curl_error($ch);
      $this->page = '';
    }

    // close cURL resource, and free up system resources
    curl_close($ch);

    if (isset($this->cookie)) {
      $_SESSION['atpconn'] = array(time(), $this->cookie);
    }
    
    dpm($this->cookie, 'Cookie after');
    
    return $this->page;
  }

  private function getPage($ch, $url, $cookie) {
    // set URL and other appropriate options
    curl_setopt($ch, CURLOPT_URL, $url);
    if($this->clear_session) {
      dpm('Clear session');
      curl_setopt($ch, CURLOPT_COOKIESESSION, $this->clear_session);
      $this->clear_session = FALSE;
    }
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: text/html'));
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
    if ($cookie === NULL) {
      curl_setopt($ch, CURLOPT_HEADER, TRUE);
    }
    else {
      curl_setopt($ch, CURLOPT_COOKIE, $cookie);
    }
    // grab URL and pass it to the browser
    return curl_exec($ch);
  }
}
