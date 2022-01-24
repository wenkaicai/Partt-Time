# PR CHECKLIST

1. Does the game build and run (after rebuild?)
2. In all games
   1. does pressing (**Q**) quit the game without error?
   2. does pressing (**D**) toggle/change it to debug mode?
   3. does pressing (**R**) reset the game, back to mini game's state?
3. Board game    
   1. roll the dice (**space**)
   2. does player 0 move?
   3. are the move correct?
   4. if at turning points, does use (**arrow keys**) work?
   5. does doge animation look correct?
   6. can a player get an item? (during roll press a number to land on a mushroom space, next turn click **B** to see if they got an item).
   6. does pressing (**B**) shows item when prompted? 
   7. does pressing (**S**) save the current player in `{CMAKE_BUILD_DIRECTORY/save.txt}`, as a line of text?
   8. does changing the `{CMAKE_BUILD_DIRECTORY/save.txt}` file and then loading it change the player's position, after pressing (**L**) to load?
   9. after loading, are the number of players the same as when it was saved?
   10. after loading, are the players in the same position as when it was saved?
   11. after loading, do the players contain the same items as when it was saved? (confirm by pressing **B**)
   12. are player rankings correct based on the points they have?
   13. do the points get properly reflected based on where a player lands? +3 blue space, -3 red space, etc.
   14. when a player lands on a spring space, do they get telported to another player?
   15. does using a mega/mini mushrom grow/shrink the player?
   16. does stepping over other players when you are big steal their coins?
   17. to the intersections with an empty location in one of the diretions prevent users from choosing that direction if they are not mini?
   18. does being mini allow you to go into these intersections
4. Mac game
   1. does use (**arrow keys**) change the player's position?
   2. does collision change the player color?
5. Planit game
   1. does the debug line follow the mouse cursor?
   2. does the player launch after pressing **space**
   3. does the player launch in the direction that the line is pointing?
   4. does the player turn green after hitting the fish from the back?
   5. does the player turn red after hitting the fish from the front?
   6. does the player turn red after hitting the planet?
6. Shower game
   1. does the player turn red when they hit a cat?
   2. does the food disappear when the player collides with it?
   3. does the enemy follow the player?
