#include <iostream>
#include "include/engine.h"
#include "include/device_manager.h"
#include <iterator>
#include <vector>
#include "include/MidiFile.h"

typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const&);

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(BYTE c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}

std::vector<BYTE> base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  BYTE char_array_4[4], char_array_3[3];
  std::vector<BYTE> ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
          ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
  }

  return ret;
}

void convert() {
    smf::MidiFile midifile;
    midifile.read("/home/nishi/Downloads/midi/bach_846.mid");
       // midifile.write("output.mid");
    std::cout << midifile.status() << std::endl;
    midifile.joinTracks();
    midifile.writeBinascWithComments("/home/nishi/Downloads/midi/bach_846_joined.mid");
}

int main() {
    // int m_notas[25] = {
    //     11, 12, 13, 14, 15,
    //     31, 32, 33, 34, 35, 
    //     21, 22, 23, 24, 25,
    //     41, 42, 43, 44, 45,
    //     51, 52, 53, 54, 55
    // };
    // convert();
    // return 0;
    std::vector<BYTE> myData;
    std::string str = "4973978b60cc44d0852666ae1347b521:a3561f1a780d48aeb9dd5feac25d1838";
    for(char& c : str) {
        myData.push_back(c);
    }
    std::string encodedData = base64_encode(&myData[0], myData.size());
    std::cout << encodedData << std::endl;
    // std::vector<BYTE> decodedData = base64_decode(encodedData);

    // std::vector<int> m_notas(25, 1);

    // Engine* engine = Engine::GetInstance(m_notas);
    // DeviceManager dm;
    // std::cout << "device id: " << dm.get_device_id() << std::endl;
    // DeviceManager dm;
    // std::cout << engine->count_notas << std::endl;
    // engine->play();
    // Markov markov(5, m);
    // markov.display();
    // nota(markov);
    // markov.proximo_estado();
    // markov.display();
    // nota(markov);
}
