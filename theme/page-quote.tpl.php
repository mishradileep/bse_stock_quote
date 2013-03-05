<?php 
if($quotes):
foreach($quotes as $quote) { ?>
<table width="100%" border="1" cellspacing="5" cellpadding="5">

	<tr>
		<td colspan="6"><h3>
				<b><?php print $quote['Scrip'][0]['scripName']?> </b> (
				<?php print $quote['Scrip'][0]['last']?>
				)
			</h3></td>
	</tr>
	<tr>
		<td><strong>Date</strong></td>
		<td><strong>Open</strong></td>
		<td><strong>Low</strong></td>
		<td><strong>Hight</strong></td>
		<td><strong>Close</strong></td>
		<td><strong>Last</strong></td>
	</tr>
	<?php foreach($quote['Scrip'] as $data) {?>
	<tr>
		<td><?php print $data['tradedDate']; ?></td>
		<td><?php print $data['open']; ?></td>
		<td><?php print $data['low']; ?></td>
		<td><?php print $data['high']; ?></td>
		<td><?php print $data['close']; ?></td>
		<td><?php print $data['last']; ?></td>
	</tr>
	<?php }?>

</table>
<?php }
else:
print "No Stock configured to display.";
if(user_access('administer bse quotes')) {
  print "Configure Quotes from " . l('admin/config/services/bse_quotes/config_quotes', 'admin/config/services/bse_quotes/config_quotes') . " to see the stock here.";
}
endif;