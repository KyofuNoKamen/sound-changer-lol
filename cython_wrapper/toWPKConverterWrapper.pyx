from libc.stdint cimport uint32_t
from libc.stdint cimport uint8_t

cdef struct audio_data:
    uint32_t id
    uint32_t length
    uint8_t* data

ctypedef audio_data AudioData

cdef extern from "toWPKConverter.c":
    cdef struct STATIC_LIST

ctypedef STATIC_LIST AudioDataList

cdef extern from "toWPKConverter.c":
    void write_wpk_file(AudioDataList* wemFiles, char* outputPath)
    void printTest()
    void openFile(char* pathToDir, char* wpkName)

def wpk_convert(wemList, outputPath):
    cdef AudioDataList* wemfiles
    write_wpk_file(wemfiles, outputPath)


def test(text):
    printTest()

def fromWemToWPK(path, wpkName):
    openFile(path, wpkName)