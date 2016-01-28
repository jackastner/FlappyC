{-# LANGUAGE ForeignFunctionInterface #-}
module GameLogic where

import Foreign.Marshal.Alloc
import Foreign.Ptr 
import Foreign.Storable
import Control.Applicative
import Foreign.Marshal.Array
#include "gamestructs.h"

data Pipe = Pipe { xPos :: Int, topEnd :: Int, bottomStart :: Int}
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
    int pipe_gap_height;
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
    pipeGapHeight :: Int,
    pipeInterval :: Int,
    pipeV :: Int,

    score :: Int,

    birdV :: Int,
    birdX :: Int,
    birdY :: Int,

    pipeArray :: [Pipe]}
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
        pg <- fromIntegral <$> {#get GameData->pipe_gap_height#} p
        pi <- fromIntegral <$> {#get GameData->pipe_interval#} p
        pv <- fromIntegral <$> {#get GameData->pipe_v#} p

        s  <- fromIntegral <$> {#get GameData->score#} p

        bv <- fromIntegral <$> {#get GameData->bird_v#} p
        bx <- fromIntegral <$> {#get GameData->bird_x#} p
        by <- fromIntegral <$> {#get GameData->bird_y#} p
        
        numPipes <- fromIntegral <$> get_num_pipes p
        pipes <- {#get GameData->pipe_array#} p >>= peekArray numPipes
        return $ GameData sw sh bw bh pw pg pi pv s bv bx by pipes
    poke p (GameData sw sh bw bh pw pg pi pv s bv bx by pipes) = do
        {#set GameData->stage_width#} p $ fromIntegral sw     
        {#set GameData->stage_height#} p $ fromIntegral sh 
        {#set GameData->bird_width#} p $ fromIntegral bw
        {#set GameData->bird_height#} p $ fromIntegral bh
        {#set GameData->pipe_width#} p $ fromIntegral pw
        {#set GameData->pipe_gap_height#} p $ fromIntegral pg
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
    p <- mallocBytes {#sizeof GameData#} :: IO (Ptr GameData)
    {#set GameData->stage_width#} p stageW
    {#set GameData->stage_height#} p stageH
    {#set GameData->bird_width#} p birdW
    {#set GameData->pipe_width#} p pipeW
    {#set GameData->pipe_v#} p pipeV
    {#set GameData->pipe_interval#} p pipeInt 
    mallocBytes pipeArrSize >>= {#set GameData->pipe_array#} p
    reset_state p
    return p
    where stageW = 100
          stageH = 100
          birdW  = 5
          pipeW  = 8
          pipeV  = 2
          pipeInt = (stageW + 2*pipeW)`div`2
          pipeArrSize = fromIntegral $ {#sizeof Pipe#} * getNumPipes stageW pipeW pipeInt 

foreign export ccall create_GameData :: IO GameDataPtr 

reset_state :: GameDataPtr -> IO ()
reset_state p = do
    {#set GameData->bird_x#} p birdx
    {#set GameData->bird_y#} p birdy
    {#set GameData->bird_v#} p birdv
    {#set GameData->score#} p score
    c <- fromIntegral <$> get_num_pipes p
    pI <- fromIntegral <$> {#get GameData->pipe_interval#} p
    sH <- fromIntegral <$> {#get GameData->stage_height#} p
    pipes <- {#get GameData->pipe_array#} p
    resetPipes pipes c pI sH
    where birdx = 50
          birdy = 50
          birdv = -5
          score = 0

resetPipes :: PipePtr -> Int -> Int -> Int -> IO ()
resetPipes p c  pI sH = pokeArray p pipes
    where pipes = map (\n-> Pipe (pI * (n+1) - 20) (sH`div`2 - 15) (sH`div`2 + 15)) [0..c-1] 


foreign export ccall reset_state ::GameDataPtr -> IO ()

destroy_GameData :: GameDataPtr -> IO ()
destroy_GameData p = (free <$> {#get GameData->pipe_array#} p) >> (free p)  

foreign export ccall destroy_GameData :: GameDataPtr -> IO ()

get_num_pipes :: GameDataPtr  -> IO C2HSImp.CInt
get_num_pipes p = do
    stageWidth <- {#get GameData->stage_width#} p
    pipeWidth <- {#get GameData->pipe_width#} p
    pipeInterval <- {#get GameData->pipe_interval#} p
    return $  getNumPipes stageWidth pipeWidth pipeInterval

getNumPipes :: Integral a => a -> a -> a -> a
getNumPipes sw pw pInt = (sw + 2*pw)`div`pInt

foreign export ccall get_num_pipes :: GameDataPtr -> IO C2HSImp.CInt

update_state :: GameDataPtr -> IO ()
update_state p = do
    numPipes <- fromIntegral <$> get_num_pipes p
    pipes <- {#get GameData->pipe_array#} p >>= peekArray numPipes 
    birdX <- fromIntegral <$> {#get GameData->bird_x#} p
    pipeV <- fromIntegral <$> {#get GameData->pipe_v#} p
    score <- fromIntegral <$> {#get GameData->score#} p
    let newScore = updateScore pipes birdX pipeV score
    {#set GameData->score#} p $ fromIntegral newScore

    birdY <- fromIntegral <$> {#get GameData->bird_y#} p
    birdV <- fromIntegral <$> {#get GameData->bird_v#} p
    let (newBirdY,newBirdV) = updateBird birdY birdV
    {#set GameData->bird_y#} p $ fromIntegral newBirdY
    {#set GameData->bird_v#} p $ fromIntegral newBirdV

    pipeW <- fromIntegral <$> {#get GameData->pipe_width#} p
    stageW <- fromIntegral <$> {#get GameData->stage_width#} p
    stageH <- fromIntegral <$> {#get GameData->stage_height#} p
    pipePtr <- {#get GameData->pipe_array#} p 
    updatePipes pipes pipeV pipeW stageW stageH >>= pokeArray pipePtr

updateScore :: [Pipe] -> Int -> Int -> Int -> Int
updateScore pipes birdX pipeV score =score +  (sum . map (fromEnum . birdInRange) $ pipes)
    where birdInRange (Pipe x _ _) = birdX > (x - pipeV) && birdX <= x

updateBird :: Int -> Int -> (Int,Int)
updateBird y v = (y+v,v+1)

updatePipes :: [Pipe] -> Int -> Int -> Int -> Int -> IO [Pipe]
updatePipes pipes pipeV pipeW stageW stageH = mapM (\p -> movePipe p pipeV pipeW stageW stageH) pipes

movePipe :: Pipe -> Int -> Int -> Int -> Int -> IO Pipe 
movePipe (Pipe x t b) pipeV pipeW stageW stageH
    | x + pipeV < -pipeW  = do 
        top <- return 10 --randomRIO (0,stageH - 30)
        let bottom = top + 30
        let newX   = stageW + pipeW
        return $ Pipe newX top bottom
    | otherwise = return $ Pipe (x - pipeV) t b  

foreign export ccall update_state :: GameDataPtr -> IO ()

get_pipe :: GameDataPtr -> C2HSImp.CInt -> IO PipePtr
get_pipe p n = flip advancePtr (fromIntegral n) <$> {#get GameData->pipe_array#} p

foreign export ccall get_pipe :: GameDataPtr -> C2HSImp.CInt  -> IO PipePtr

get_score :: GameDataPtr -> IO C2HSImp.CInt
get_score = {#get GameData->score#}

foreign export ccall get_score ::GameDataPtr -> IO C2HSImp.CInt

get_stage_height :: GameDataPtr -> IO C2HSImp.CInt
get_stage_height = {#get GameData->stage_height#}

foreign export ccall get_stage_height :: GameDataPtr -> IO C2HSImp.CInt

get_stage_width :: GameDataPtr -> IO C2HSImp.CInt
get_stage_width = {#get GameData->stage_width#}

foreign export ccall get_stage_width :: GameDataPtr -> IO C2HSImp.CInt

get_bird_y :: GameDataPtr -> IO C2HSImp.CInt
get_bird_y = {#get GameData->bird_y#}

foreign export ccall get_bird_y :: GameDataPtr -> IO C2HSImp.CInt

get_bird_x :: GameDataPtr -> IO C2HSImp.CInt
get_bird_x = {#get GameData->bird_x#}

foreign export ccall get_bird_x :: GameDataPtr -> IO C2HSImp.CInt

is_gameover :: GameDataPtr -> IO Bool
is_gameover p  = do
    y <- {#get GameData->bird_y#} p
    h <- {#get GameData->stage_height#} p
    return $ y < 0 || y >= h

foreign export ccall is_gameover :: GameDataPtr -> IO Bool

flap_bird :: GameDataPtr -> IO ()
flap_bird  p = {#set GameData->bird_v#} p (-5)

foreign export ccall flap_bird :: GameDataPtr -> IO ()
