{-# LANGUAGE DeriveGeneric #-}
-- comp2209 Functional Programming Challenges
-- (c) University of Southampton 2021
-- by Yaqin Hasan
-- Skeleton code to be updated with your solutions
-- The dummy functions here simply return an arbitrary value that is usually wrong

-- DO NOT MODIFY THE FOLLOWING LINES OF CODE
module Challenges (Atoms,Interactions,Pos,EdgePos,Side(..),Marking(..),
                   LamExpr(..),LetExpr(..),CLExpr(..),
                   calcBBInteractions,
                   solveBB,
                   prettyPrint,
                   parseLet,
                   clTransform,
                   innerRedn1,outerRedn1,innerCLRedn1,outerCLRedn1,compareInnerOuter
                   )
where


-- Import standard library and parsing definitions from Hutton 2016, Chapter 13
import Data.Char
import Parsing
import Control.Monad
import Data.List
import GHC.Generics (Generic,Generic1)
import Control.DeepSeq


instance NFData CLExpr
instance NFData LetExpr
instance NFData LamExpr
instance NFData Marking
instance NFData Side


-- test function for testing testing using Tests.hs

fact :: Int -> Int
fact i
  | i > 0 = i * fact (i - 1)
  | i == 0 = 1
  | otherwise = -1

-- Challenge 1
-- Calculate Interactions in the Black Box

type Atoms = [ Pos ]
type Interactions = [  ( EdgePos , Marking )  ]
type Pos = (Int, Int)   -- top left is (1,1) , bottom right is (N,N) where N is size of grid
type EdgePos = ( Side , Int ) -- int range is 1 to N where N is size of grid

data Side = North | East | South | West
            deriving (Show, Eq, Ord)

data Marking =  Absorb | Reflect | Path EdgePos
                deriving (Show, Eq)

getSide :: EdgePos -> Side
getSide (side, _) = side

getEdgeIndex :: EdgePos -> Int
getEdgeIndex (_, int) = int

oppositeSide :: Side -> Side
oppositeSide North = South
oppositeSide South = North
oppositeSide East = West
oppositeSide West = East

getAllEdgePos :: Int -> [EdgePos]
getAllEdgePos size = [(side, i) | side <- [North, East, South, West], i <- [1..size]]

-- gets the initial path of the ray
getInitialPath :: Int -> EdgePos -> [Pos]
getInitialPath size (North, int) = [(n, int) | n <- [1..size]]
getInitialPath size (East, int) = [(int, n) | n <- [1..size]]
getInitialPath size (South, int) = [(n, int) | n <- [size..1]]
getInitialPath size (West, int) = [(int, n) | n <- [size..1]]

-- gets adjacent positions to the initial path
getAdj :: Int -> EdgePos -> ([Pos], [Pos])
getAdj size (North, int) = ([(n, i) | let i = int + 1, 0 <= i, i <= size, n <- [1..size]], [(n, i) | let i = int - 1, 0 <= i, i <= size, n <- [1..size]])
getAdj size (East, int) = ([(i, n) | let i = int + 1, 0 <= i, i <= size, n <- [1..size]], [(i, n) | let i = int - 1, 0 <= i, i <= size, n <- [1..size]])
getAdj size (South, int) = ([(n, i) | let i = int - 1, 0 <= i, i <= size, n <- [1..size]], [(n, i) | let i = int + 1, 0 <= i, i <= size, n <- [1..size]])
getAdj size (West, int) = ([(i, n) | let i = int - 1, 0 <= i, i <= size, n <- [1..size]], [(i, n) | let i = int + 1, 0 <= i, i <= size, n <- [1..size]])

-- not to mix up which is left and right (relative to starting position)
getLeftAdj :: ([Pos], [Pos]) -> [Pos]
getLeftAdj adj = fst adj

getRightAdj :: ([Pos], [Pos]) -> [Pos]
getRightAdj adj = snd adj

getAllAdj :: ([Pos], [Pos]) -> [Pos]
getAllAdj (l1, l2) = l1 ++ l2

-- check if the length of the intersection between atoms and the edge reflected tiles is > 0
checkEdgeReflection :: Int -> Atoms -> EdgePos -> Bool
checkEdgeReflection size atoms (North, int) = length (intersect atoms [(1, i) | i <- [int + 1, int - 1], 1 <= i, i <= size]) > 0
checkEdgeReflection size atoms (East, int) = length (intersect atoms [(i, 1) | i <- [int + 1, int - 1], 1 <= i, i <= size]) > 0
checkEdgeReflection size atoms (South, int) = length (intersect atoms [(size, i) | i <- [int + 1, int - 1], 1 <= i, i <= size]) > 0
checkEdgeReflection size atoms (West, int) = length (intersect atoms [(i, size) | i <- [int + 1, int - 1], 1 <= i, i <= size]) > 0

-- check intersection between atoms and path
checkIntersection :: Atoms -> [Pos] -> Bool
checkIntersection atoms path = length (intersect atoms path) > 0

-- figure out the direction of deflection
getDeflectDirection :: Int -> Atoms -> EdgePos -> Side
getDeflectDirection size atoms ePos
  | getSide ePos == North && checkIntersection atoms leftAdj == True = West
  | getSide ePos == North && checkIntersection atoms rightAdj == True = East
  | getSide ePos == East && checkIntersection atoms leftAdj == True = North
  | getSide ePos == East && checkIntersection atoms rightAdj == True = South
  | getSide ePos == South && checkIntersection atoms leftAdj == True = East
  | getSide ePos == South && checkIntersection atoms rightAdj == True = West
  | getSide ePos == West && checkIntersection atoms leftAdj == True = South
  | getSide ePos == West && checkIntersection atoms rightAdj == True = North
  where
    leftAdj = getLeftAdj $ getAdj size ePos
    rightAdj = getRightAdj $ getAdj size ePos

-- get the marking (end result) for each ray
getMarking :: Int -> Atoms -> EdgePos -> Marking
getMarking size atoms ePos
  | checkIntersection atoms (getInitialPath size ePos) == True = Absorb
  | checkEdgeReflection size atoms ePos == True = Reflect
  | checkIntersection atoms (getAllAdj $ getAdj size ePos) == True = (Path ((getDeflectDirection size atoms ePos), 1))
  | otherwise = (Path ((oppositeSide $ getSide ePos), getEdgeIndex ePos))

calcBBInteractions :: Int -> Atoms -> Interactions
calcBBInteractions size atoms = [((ePos), getMarking size atoms ePos) | ePos <- getAllEdgePos size ]

-- Challenge 2
-- Solve a Black Box

-- simplify marking to absorb or not absorb, used in testing/development
getSimplifiedMarking :: Int -> Atoms -> EdgePos -> Marking
getSimplifiedMarking size atoms ePos
  | checkIntersection atoms (getInitialPath size ePos) == True = Absorb
  | otherwise = Reflect -- not neccessarily

-- modified function, used in interactive testing
calcSimplifiedBBInteractions :: Int -> Atoms -> Interactions
calcSimplifiedBBInteractions size atoms = [((ePos), getSimplifiedMarking size atoms ePos) | ePos <- getAllEdgePos size ]

-- filtering out all non absorb type markings
filterInters :: ( EdgePos , Marking ) -> Bool
filterInters ((North, _), Absorb) = True
filterInters ((East, _), Absorb) = True
filterInters (_, _) = False

cleanupInteractions :: Interactions -> Interactions
cleanupInteractions inters = filter filterInters inters

-- using the absorb to figure out possible locations for the atoms
possiblePos :: Int -> (EdgePos, Marking) -> [Pos]
possiblePos size ((North, int), Absorb) = [(i, int) | i <- [2..size]]
possiblePos size ((East, int), Absorb) = [(int, i) | i <- [2..size]]

solveBB :: Int -> Interactions -> Atoms
solveBB size inters =
  let positions = [pos | pos <- concat (map (possiblePos size) (cleanupInteractions inters))]
  in positions \\ (nub positions)

-- Challenge 3
-- Pretty Printing Lambda with Scott Numerals

data LamExpr =  LamApp LamExpr LamExpr  |  LamAbs Int LamExpr  |  LamVar Int
                deriving (Eq, Show, Read)

prettyPrint :: LamExpr -> String
prettyPrint (LamVar i) = "x" ++ show i
prettyPrint (LamAbs i le) = "\\x" ++ show i ++ " -> " ++ (prettyPrint le)
prettyPrint (LamApp (LamVar i1) (LamVar i2)) = "x" ++ show i1 ++ "x" ++ show i2
prettyPrint (LamApp (LamVar i1) le) = "x" ++ show i1 ++ " " ++ (prettyPrint le)
prettyPrint (LamApp le (LamVar i2)) = (prettyPrint le) ++ " x" ++ show i2
prettyPrint (LamApp (LamAbs i1 le1) (LamAbs i2 le2)) = (prettyPrint (LamAbs i1 le1)) ++ " " ++ (prettyPrint (LamAbs i2 le2))
prettyPrint (LamApp (LamAbs i1 le1) le2) = (prettyPrint (LamAbs i1 le1)) ++ " " ++ (prettyPrint le2)
prettyPrint (LamApp le1 (LamAbs i1 le2)) = (prettyPrint le1) ++ " " ++ (prettyPrint (LamAbs i1 le2))
prettyPrint (LamApp le1 le2) = "(" ++ (prettyPrint le1) ++ ") (" ++ (prettyPrint le2) ++ ")"


-- Challenge 4
-- Parsing Let Expressions

data LetExpr =  LetApp LetExpr LetExpr |
                LetDef [([Int], LetExpr)] LetExpr |
                LetFun Int |
                LetVar Int |
                LetNum Int
                deriving (Show, Eq)

parseLet :: String -> Maybe LetExpr
parseLet "" = Nothing
parseLet s = Just (fst $ head $ parse (parseLetAux) s)

getNum :: LetExpr -> Int
getNum (LetNum i) = i
getVar (LetVar i) = i
getFun (LetFun i) = i

parseLetAux =
  do
    char 'x'
    i1 <- (some digit)
    char ' '
    char 'x'
    i2 <- (some digit)
    return (LetApp (LetVar (read i1 :: Int)) (LetVar(read i2 :: Int)))
    -- failed recursive call attempt
    --return (LetApp (parseLetAux ("x" ++ i1)) (parseLetAux ("x" ++ i2)))
  <|>
  do
    i <- (some digit)
    return (LetNum (read i :: Int))
  <|>
  do
    char 'x'
    i <- (some digit)
    return (LetVar (read i :: Int))
  <|>
  do
    char 'f'
    i <- (some digit)
    return (LetFun (read i :: Int))

-- Challenge 5
-- Encode lambda terms as combinators

data CLExpr = S | K  | I | CLVar Int | CLApp CLExpr CLExpr
              deriving (Show,Read,Eq)

clTransform :: LamExpr -> CLExpr
clTransform _ =  CLVar 0

-- Challenge 6
-- Compare Innermost and Outermost Reduction for Lambda and Combinators

outerRedn1 :: LamExpr -> Maybe LamExpr
outerRedn1 _ = Just (LamVar 0)

innerRedn1 :: LamExpr -> Maybe LamExpr
innerRedn1 _ = Just (LamVar 0)

outerCLRedn1 :: CLExpr -> Maybe CLExpr
outerCLRedn1 _ = Just (CLVar 0)

innerCLRedn1 :: CLExpr -> Maybe CLExpr
innerCLRedn1 _ = Just (CLVar 0)

compareInnerOuter :: LamExpr -> Int -> (Maybe Int,Maybe Int,Maybe Int,Maybe Int)
compareInnerOuter _ _ = (Just 0, Just 0, Just 0, Just 0)
