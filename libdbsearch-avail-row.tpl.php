<?php

/*
 * @file
 * One row of Work availability in chosen city / municipality library.
 * 
 * $kirjastonimi  Library name in chosen city / municipality.
 * $osasto        Deparment
 * $hylly         Shelf where placed
 * $luokka        Class of the work
 * $niteita       Count of works in library
 * $lainattavissa Count of works to be borrowed
 * $tilattu       Count of ordered works
 * $erapaiva      First due date
 * 
 * $zebra         Odd or even row.
 */
?>
  
  <tr class="<?php print $zebra; ?>">
   <td colspan="7"><?php print $kirjastonimi; ?></td>
  </tr>
  
  <?php for ($i = 0; $i < count($osasto); $i++ ) { ?>
  <tr class="<?php print $zebra; ?>">
   <td><?php print $osasto[$i]; ?></td>
   <td><?php print $hylly[$i]; ?></td>
   <td><?php print $luokka[$i]; ?></td>
   <td><?php print $niteita[$i]; ?></td>
   <td><?php print $lainattavissa[$i]; ?></td>
   <td><?php print $tilattu[$i]; ?></td>
   <td><?php print $erapaiva[$i]; ?></td>
  </tr>
  <?php } ?>
  