<?php 
/**
 * @file
 * Important user functions for the BSE Quotes module.
 */

/**
 * Generate content for BSE Quote Page
 */
function bse_stock_quotes() {

  $items = array();

  $query = db_select('bse_stock_quote', 'bse_stock_quote')
  ->condition('status', 1)
  ->range(0, 20)
  ->fields('bse_stock_quote', array(
      'id',
      'stock_code',
      'stock_name',
      'status',
  ));

  $results = $query->execute();
  $stock_quote = array();
  while ($record = $results->fetchAssoc()) {
    $stock_quote[$record['stock_code']] = _bse_stock_quote_get_stock_quote($record['stock_code']);
  }

  return theme('page-quote', array('quotes' => $stock_quote));

}