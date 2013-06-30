<?php

/*
 * @file
 * One row of availability in city / municipality libraries.
 * 
 * $kokoelma      City / municipality library where work is found
 * $niteita       Count of works in library
 * $lainattavissa Count of works to be borrowed
 * $tilattu       Count of ordered works
 * $erapaiva      First due date
 * 
 * $zebra         Odd or even row.
 */
?>
  
  <tr class="<?php print $zebra; ?>">
   <td><?php print $kokoelma; ?></td>
   <td><?php print $niteita; ?></td>
   <td><?php print $lainattavissa; ?></td>
   <td><?php print $tilattu; ?></td>
   <td><?php print $erapaiva; ?></td>
  </tr>
  