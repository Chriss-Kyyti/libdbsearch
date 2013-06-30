<?php
/* 
* @file
 * Libdbsearch work result page row template.
 * 
 * $theader       Work info row description header
 * $tdata         Work info row data
 * 
 * $zebra         Odd or even row.
*/

?>
 <tr class="<?php print $zebra; ?>">
   <td><?php print $theader; ?></td>
   <td><?php print $tdata; ?></td>
 </tr>
