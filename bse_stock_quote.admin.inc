<?php
/**
 * @file
 * Important administration functions for the BSE Quotes module.
 */

/**
 * config form for BSE quote
 */
function bse_stock_quote_config_form() {
  $form['bse_quote_code'] = array(
      '#type' => 'textfield',
      '#title' => 'Stock Code',
      '#description' => 'Type stock name and select your desired stock. It will be automatically converted into stock code.',
      '#autocomplete_path' => 'admin/config/services/bse-stock-quote/bse-stock-quote-names/autocomplete',
      '#required' => TRUE
  );
  $form['bse_quote_weight'] = array(
      '#type' => 'weight',
      '#title' => 'Weight',
      '#description' => 'Weight in listing',
      '#required' => TRUE
  );
  $form['bse_quote_config_form_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Submit',
  );
  return $form;
}

/**
 * Autocomplete functionality stock quote
 */
function _bse_stock_quote_names_autocomplete($input_string) {
  $string = strtoupper(check_plain($input_string));
  
  $stock_mapping = _get_stock_quotes_mapping_array();
  $matches = array();
  foreach ($stock_mapping as $stock_code => $stock_name) {
    if (strpos($stock_name, $string) !== FALSE) {
      $matches[$stock_code] = $stock_name;
    }
  }
  drupal_json_output($matches);
}

/**
 * Config form validation
 */
function bse_stock_quote_config_form_validate($form_id, &$form_values) {
  $stock_mapping = _get_stock_quotes_mapping_array();
  $input_bse_quote_code = filter_xss($form_values['values']['bse_quote_code']);

  if (!array_key_exists($input_bse_quote_code, $stock_mapping)) {
    form_set_error('bse_quote_code', t('This stock code does not exist. Please select stock from autocomplete options only.'));
  }

  /* Check if code does not exist */
  $query = db_select('bse_stock_quote', 'bse');
  $query
  ->condition('bse.stock_code', $input_bse_quote_code, '=')
  ->fields('bse', array('id'))
  ->range(0, 5);
  $result = $query->execute();
  if ($result->rowCount() > 0) {
    form_set_error('bse_quote_code', t('This stock code already configured to display.'));
  }
}

/**
 * Get script name mapping with stock code
 * Read stockcode.txt file and return unserialized array
 * @return
 *   Array
 */
function _get_stock_quotes_mapping_array() {
  $my_data = &drupal_static(__FUNCTION__);

  if (!isset($my_data)) {
    if ($cache = cache_get('stock_mapping')) {
      $stock_mapping = $cache->data;
    }
    else {
      $stock_codes_file = drupal_get_path('module', 'bse_stock_quote') . '/stockcode.txt';
      $handle = fopen($stock_codes_file, "r");
      $stock_mapping = fread($handle, filesize($stock_codes_file));
      fclose($handle);
      cache_set('stock_mapping', $stock_mapping, 'cache');
    }
  }
  return unserialize($stock_mapping);
}

/**
 * Sumbit BSE Quote config form
 * Save configured stock data in database
 */
function bse_stock_quote_config_form_submit($form_id, &$form_values) {

  $stock_mapping = _get_stock_quotes_mapping_array();
  $input_bse_quote_code = filter_xss($form_values['values']['bse_quote_code']);

  $bse_quote_id = db_insert('bse_stock_quote')
  ->fields(array(
      'stock_code' => $input_bse_quote_code,
      'stock_name' => $stock_mapping[$input_bse_quote_code],
      'status' => 1,
  ))
  ->execute();

  drupal_set_message(t('Stock added.'), 'status');
}

/**
 * List BSE stock code in Tableselect to perform admin oprtaions like
 * Publish, Unpublish, Delete Etc
 */
function bse_stocks_quote_list() {
  //Create a list of headers for your Html table (see Drupal 7 docs for theme_table here)
  $header = array(
      'stock_code' => array('data' => t('Stock Code'), 'field' => 'bse_stock_quote.stock_code', 'sort' => 'asc'),
      'stock_name' => array('data' => t('Stock Title'), 'field' => 'bse_stock_quote.stock_name', 'sort' => 'asc'),
      'status' => t('status'),
  );

  //Create the Sql query. This uses various parts of Drupal 7's new DBTNG database abstraction layer.
  $query = db_select('bse_stock_quote', 'bse_stock_quote')
  ->extend('PagerDefault')  //Pager Extender
  ->limit(20)    //results per page
  ->extend('TableSort')  //Sorting Extender
  ->orderByHeader($header)//Field to sort on is picked from $header
  ->fields('bse_stock_quote', array(
      'id',
      'stock_code',
      'stock_name',
      'status',
  ));
  $results = $query
  ->execute();

  $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Update options'),
      '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('bse_stock_quote_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }

  $form['options']['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => 'unblock',
  );
  $options = array();
  $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
  );


  $rows = array();

  foreach ($results as $stock) {
    $rows[$stock->id] = array(
        'stock_code' => check_plain($stock->stock_code),
        'stock_name' => check_plain($stock->stock_name),
        'status' => ($stock->status)?'Published':'Unpublished',
        //'#attributes' => array('class' => array('article-row')),
    );
  }
  $form['bse_stocks_quote_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $rows,
      '#prefix' => '<table><tr><td>',
      '#suffix' => '</td></tr>',
      '#attributes' => array('class' => array('article-row')),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

/**
 * Submit the bse_stocks_list administration update form.
 */
function bse_stocks_quote_list_submit($form, &$form_state) {

  $operations = module_invoke_all('bse_stock_quote_operations', $form, $form_state);
  $operation = $operations[$form_state['values']['operation']];

  // Filter out unchecked links.
  $quotes = array_filter($form_state['values']['bse_stocks_quote_table']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($quotes), $operation['callback arguments']);
    }
    else {
      $args = array($quotes);
    }
    call_user_func_array($function, $args);
    drupal_set_message(t('The update has been performed.'));
  }
}

/**
 * Implementation of HOOK_VALIDATE for bse_stocks_list.
 */
function bse_stocks_quote_list_validate($form, &$form_state) {

  $form_state['values']['bse_stocks_quote_table'] = array_filter($form_state['values']['bse_stocks_quote_table']);

  if (count($form_state['values']['bse_stocks_quote_table']) == 0) {
    form_set_error('', t('No quote selected.'));
  }
}