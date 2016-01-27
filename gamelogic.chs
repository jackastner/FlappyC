{-# LANGUAGE ForeignFunctionInterface #-}
module GameLogic where

import Foreign.Marshal.Alloc
import Foreign.Ptr 
import Foreign.Storable
import Control.Applicative
import Foreign.Marshal.Array
#include "gamestructs.h"

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
data GameData
{#pointer *GameData as GameDataPtr -> GameData#}

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
