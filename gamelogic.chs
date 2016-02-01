{-# LANGUAGE ForeignFunctionInterface #-}
module GameLogic where

import Foreign.C.Types
import Foreign.Marshal.Alloc
import Foreign.Ptr 
import Foreign.Storable
import Control.Applicative
import Foreign.Marshal.Array
#include "gamestructs.h"

data Pipe = Pipe { xPos :: Int, topEnd :: Int, bottomStart :: Int} deriving (Show)
{#pointer *Pipe as PipePtr -> Pipe#}

instance Storable Pipe where
    sizeOf _ = {#sizeof Pipe#}
    alignment _ = {#alignof Pipe#}
    peek p = do
        x <- fromIntegral <$> {#get Pipe->x#} p
        t <-  fromIntegral <$> {#get Pipe->top_end#} p
        b <- fromIntegral <$> {#get Pipe->bottom_start#} p
        return $ Pipe x t b 
    poke p pipe = do
        {#set Pipe->x#} p $ (fromIntegral $ xPos pipe)
        {#set Pipe->top_end#} p $ (fromIntegral $ topEnd pipe) 
        {#set Pipe->bottom_start#} p $ (fromIntegral $ bottomStart pipe)
#c
struct GameData {
    int stage_width;
    int stage_height;

    int bird_width;
    int bird_height;

    int pipe_width;
    int pipe_interval;
    int pipe_v;

    int score;

    int bird_v;
    int bird_x;
    int bird_y;

    Pipe *pipe_array;
};
#endc
data GameData = GameData {
    stageWidth :: Int,
    stageHeight :: Int,
    
    birdWidth :: Int,
    birdHeight :: Int,

    pipeWidth :: Int,
    pipeInterval :: Int,
    pipeV :: Int,

    score :: Int,

    birdV :: Int,
    birdX :: Int,
    birdY :: Int,

    pipeArray :: [Pipe]} deriving (Show)
{#pointer *GameData as GameDataPtr -> GameData#}
instance Storable GameData where
    sizeOf _ = {#sizeof GameData#}
    alignment _ = {#alignof GameData#}
    peek p = do
        sw <- fromIntegral <$> {#get GameData->stage_width#} p
        sh <- fromIntegral <$> {#get GameData->stage_height#} p
        bw <- fromIntegral <$> {#get GameData->bird_width#} p
        bh <- fromIntegral <$> {#get GameData->bird_height#} p
        pw <- fromIntegral <$> {#get GameData->pipe_width#} p
        pi <- fromIntegral <$> {#get GameData->pipe_interval#} p
        pv <- fromIntegral <$> {#get GameData->pipe_v#} p
        s  <- fromIntegral <$> {#get GameData->score#} p
        bv <- fromIntegral <$> {#get GameData->bird_v#} p
        bx <- fromIntegral <$> {#get GameData->bird_x#} p
        by <- fromIntegral <$> {#get GameData->bird_y#} p
        let game = GameData sw sh bw bh pw pi pv s bv bx by [] 
        pipes <- {#get GameData->pipe_array#} p >>= (peekArray $ numPipes game)
        return $ game {pipeArray = pipes}

    poke p (GameData sw sh bw bh pw pi pv s bv bx by pipes) = do
        {#set GameData->stage_width#} p $ fromIntegral sw     
        {#set GameData->stage_height#} p $ fromIntegral sh 
        {#set GameData->bird_width#} p $ fromIntegral bw
        {#set GameData->bird_height#} p $ fromIntegral bh
        {#set GameData->pipe_width#} p $ fromIntegral pw
        {#set GameData->pipe_interval#} p $ fromIntegral pi
        {#set GameData->pipe_v#} p $ fromIntegral pv
        {#set GameData->score#} p $ fromIntegral s
        {#set GameData->bird_v#} p $ fromIntegral bv
        {#set GameData->bird_x#} p $ fromIntegral bx
        {#set GameData->bird_y#} p $ fromIntegral by
        pipePtr <- {#get GameData->pipe_array#} p
        pokeArray pipePtr pipes 

create_GameData :: IO GameDataPtr 
create_GameData = do
    gamePtr <- mallocBytes {#sizeof GameData#} :: IO (Ptr GameData)
    pipePtr <- mallocBytes ({#sizeof Pipe#} * (length . pipeArray $ initialGameState)) 
    {#set GameData->pipe_array#} gamePtr pipePtr
    poke gamePtr initialGameState
    return gamePtr

initialGameState :: GameData
initialGameState = resetState $  GameData {
        stageWidth  = 100,
        stageHeight = 100,

        birdHeight = 5,
        birdWidth = 5,
        
        pipeWidth = 8,
        pipeInterval = (100+2*8)`div`2,
        pipeV = 2}

foreign export ccall create_GameData :: IO GameDataPtr 

reset_state :: GameDataPtr -> IO ()
reset_state p = resetState <$> peek p >>= poke p

resetState :: GameData -> GameData
resetState game = resetPipes $ game {
    birdX = 50,
    birdY = 50,
    birdV = -5,
    score = 0}

resetPipes :: GameData -> GameData
resetPipes game = game {pipeArray = map resetPipe [0..c-1]}
    where resetPipe n = Pipe ((pipeInterval game) * (n+1) - 20) top bot
          top = (stageHeight game)`div`2 - 15
          bot = (stageHeight game)`div`2 + 15
          c = numPipes game

foreign export ccall reset_state ::GameDataPtr -> IO ()

destroy_GameData :: GameDataPtr -> IO ()
destroy_GameData p = (free <$> {#get GameData->pipe_array#} p) >> (free p)  

foreign export ccall destroy_GameData :: GameDataPtr -> IO ()
 
get_num_pipes :: GameDataPtr  -> IO CInt
get_num_pipes p = fromIntegral . numPipes <$> peek p  

numPipes :: GameData -> Int
numPipes game = ((stageWidth game) + 2*(pipeWidth game))`div`(pipeInterval game)  
    
foreign export ccall get_num_pipes :: GameDataPtr -> IO CInt

update_state :: GameDataPtr -> IO ()
update_state p = updateState <$> peek p >>= poke p

updateState :: GameData -> GameData
updateState = updatePipes . updateBird . updateScore 

updateScore :: GameData -> GameData
updateScore game = game {score = (score game) + (fromEnum . any birdInRange . pipeArray $ game)}
    where birdInRange (Pipe x _ _) = (birdX game) > (x - (pipeV game)) && (birdX game) <= x

updateBird :: GameData -> GameData
updateBird game = game {birdY = birdY game + birdV game, birdV = birdV game + 1}

updatePipes :: GameData -> GameData
updatePipes game = game {pipeArray = map (movePipe game) (pipeArray game)}

movePipe :: GameData -> Pipe -> Pipe
movePipe game (Pipe x t b)  
    | x + (pipeV game) < -(pipeWidth game) = Pipe (pipeWidth game + stageWidth game) 10 40 
    | otherwise = Pipe (x - pipeV game) t b

foreign export ccall update_state :: GameDataPtr -> IO ()

get_pipe :: GameDataPtr -> CInt -> IO PipePtr
get_pipe p n = flip advancePtr (fromIntegral n) <$> {#get GameData->pipe_array#} p

foreign export ccall get_pipe :: GameDataPtr -> CInt  -> IO PipePtr

get_score :: GameDataPtr -> IO CInt
get_score = {#get GameData->score#}

foreign export ccall get_score ::GameDataPtr -> IO CInt

get_stage_height :: GameDataPtr -> IO CInt
get_stage_height = {#get GameData->stage_height#}

foreign export ccall get_stage_height :: GameDataPtr -> IO CInt

get_stage_width :: GameDataPtr -> IO CInt
get_stage_width = {#get GameData->stage_width#}

foreign export ccall get_stage_width :: GameDataPtr -> IO CInt

get_bird_y :: GameDataPtr -> IO CInt
get_bird_y = {#get GameData->bird_y#}

foreign export ccall get_bird_y :: GameDataPtr -> IO CInt

get_bird_x :: GameDataPtr -> IO CInt
get_bird_x = {#get GameData->bird_x#}

foreign export ccall get_bird_x :: GameDataPtr -> IO CInt

is_gameover :: GameDataPtr -> IO Bool
is_gameover p  = isGameOver <$> peek p

isGameOver :: GameData -> Bool
isGameOver game@GameData{birdY=y,stageHeight=h,pipeArray=pipes} = y < 0 || y >=  h || any (pipeCollision game) pipes 

pipeCollision :: GameData -> Pipe -> Bool
pipeCollision game pipe = xHit && yHit
    where xHit = (birdX game) > (xPos pipe) - (pipeWidth game - birdWidth game)`div`2 &&
                 (birdX game) < (xPos pipe) + (pipeWidth game + birdWidth game)`div`2
          yHit = (birdY game) > bottomStart pipe - (birdHeight game)`div`2 ||
                 (birdY game) < topEnd pipe + (birdHeight game)`div`2

foreign export ccall is_gameover :: GameDataPtr -> IO Bool

flap_bird :: GameDataPtr -> IO ()
flap_bird  p = {#set GameData->bird_v#} p (-5)
foreign export ccall flap_bird :: GameDataPtr -> IO ()
