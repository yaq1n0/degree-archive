-- comp2209 Functional Programming Challenges
-- by Yaqin Hasan
-- (c) University of Southampton 2021

import Challenges

-- testing import from Challenges.hs, simple dummy test
factTest :: (Int -> Int) -> Bool -- takes a function of int to int type and returns boolean
factTest f =
  f 0 == 1 &&
  f 1 == 1 &&
  f 2 == 2 &&
  f 5 == 120 &&
  f (-1) == -1 && -- testing the error cases with -1
  f (-6) == -1

-- copied from instructions
calcBBInteractionsTest :: (Int -> Atoms -> Interactions) -> Bool
calcBBInteractionsTest c =
  c 8 [ (2,3) , (7,3) , (4,6) , (7,8) ] == "[((North,1),Path (West,2)),((North,2),Absorb),
((North,3),Path (North,6)),((North,4),Absorb),
((North,5),Path (East,5)),((North,6),Path (North,3)),
((North,7),Absorb),((North,8),Path (East,2)),
((East,1),Path (West,1)),((East,2),Path (North,8)),
((East,3),Absorb),((East,4),Path (East,7)),
((East,5),Path (North,5)),((East,6),Absorb),
((East,7),Path (East,4)),((East,8),Absorb),
((South,1),Path (West,4)),((South,2),Absorb),
((South,3),Path (West,7)),((South,4),Absorb),
((South,5),Path (West,5)),((South,6),Reflect),
((South,7),Absorb),((South,8),Reflect),
((West,1),Path (East,1)),((West,2),Path (North,1)),
((West,3),Absorb),((West,4),Path (South,1)),
((West,5),Path (South,5)),((West,6),Absorb),
((West,7),Path (South,3)),((West,8),Absorb)]"

possiblePosTest :: (Int -> (EdgePos, Marking) -> [Pos] ) -> Bool
possiblePosTest p =
  p 8 ((North, 3), Absorb) == [(2,3),(3,3),(4,3),(5,3),(6,3),(7,3),(8,3)]

prettyPrintTest :: (LamExpr -> String) -> Bool
prettyPrintTest p =
  p LamAbs 1 (LamApp (LamVar 1) (LamAbs 1 (LamVar 1))) == "\\x1 -> x1 \\x1 -> x1"
