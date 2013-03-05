<?php if($quotes): ?>
<table width="100%" border="1" cellspacing="5" cellpadding="5">
	<tr>
		<th>Script Title</th>
		<th>Quote</th>
	</tr>
	<?php foreach($quotes as $quote) { ?>
	<tr>
		<td><?php print $quote['Scrip'][0]['scripName']?></td>
		<td><?php print $quote['Scrip'][0]['last']?></td>
	</tr>
	<?php } ?>
</table>
<?php else:
print "No Stock configured to display. Configure Quotes from " . l('admin/config/services/bse_quotes/config_quotes', 'admin/config/services/bse_quotes/config_quotes') . " to see the stock here.";
endif;