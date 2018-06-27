{ title - a unit responsible for holding information about the title screen(s).
                
          Copyright (c) 2018, Shaun Brandt / Holy Meatgoat Software
          All rights reserved.

          Redistribution and use in source and binary forms, with or without
          modification, are permitted provided that the following conditions are met:
              * Redistributions of source code must retain the above copyright
                notice, this list of conditions and the following disclaimer.
              * Redistributions in binary form must reproduce the above copyright
                notice, this list of conditions and the following disclaimer in the
                documentation and/or other materials provided with the distribution.
              * Neither the name of Holy Meatgoat Software nor the
                names of its contributors may be used to endorse or promote products
                derived from this software without specific prior written permission.

          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
          DISCLAIMED. IN NO EVENT SHALL HOLY MEATGOAT SOFTWARE BE LIABLE FOR ANY
          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       
}
unit title;

interface

const
  g_titleScreen: array[0..639] of Byte=(
     9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 
     9,  9,  9,  9,  9 , 9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    11, 12, 11, 12, 11, 13, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15,
    14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 16, 12, 11, 12, 11, 12,
    17, 18, 17, 18, 17, 19, 20,  9, 21, 22, 21, 23,  9, 24, 20, 25, 
    26, 22,  9,  9, 27, 28, 20, 27, 29,  9, 30, 18, 17, 18, 17, 18, 
    11, 12, 11, 12, 11, 31, 32, 33, 34, 35, 36,  9, 37, 36, 32, 38, 
    39,  9, 40, 41, 38, 42,  9, 37,  9,  9, 30, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 19, 43, 44, 45, 46, 47, 48, 49, 47, 50, 51, 
    52, 53, 54, 55, 56, 57,  9, 58,  9, 59, 30, 18, 17, 18, 17, 18, 
    11, 12, 11, 12, 11, 31, 60,  9, 61, 62, 61, 63,  9, 61, 62, 64, 
    65, 62,  9,  9, 66, 67,  9, 68, 69, 70, 30, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 19,  9,  9,  9,  9,  9, 24,  9, 25, 29, 20,
     9, 24,  9, 71, 72,  9,  9,  9,  9,  9, 30, 18, 17, 18, 17, 18,
    11, 12, 11, 12, 11, 31,  9,  9,  9,  9,  9, 73, 74, 75, 76, 77, 
    37, 73, 74, 37, 78,  9,  9,  9,  9,  9, 30, 12, 11, 12, 11, 12,
    17, 18, 17, 18, 17, 19,  9,  9,  9,  9,  9, 79, 80, 51, 76, 81, 
    58, 79, 80, 58, 82,  9,  9,  9,  9,  9, 30, 18, 17, 18, 17, 18, 
    11, 12, 11, 12, 11, 83, 10, 10, 10, 10, 10, 84, 10, 85, 86, 87, 
    88, 84, 10, 88, 89, 10, 10, 10, 10, 10, 90, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 91, 92, 93, 92, 93, 92, 93, 92, 93, 92, 93,
    92, 93, 92, 93, 92, 93, 92, 93, 92, 93, 92, 18, 17, 18, 17, 18,
    11, 12, 11, 12, 11, 12, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 
    104, 105, 106, 107, 108, 109, 110, 96, 111, 112, 11, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 18, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 
    123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 17, 18, 17, 18, 17, 18, 
    11, 12, 11, 12, 11, 133, 134, 135, 134, 135, 136, 137, 138, 139, 140, 141, 
    142, 143, 144, 145, 146, 147, 134, 135, 134, 135, 148, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 149,  9,  9,  9,  9, 150, 151, 152, 153, 154, 155, 
    156, 157, 158, 159, 160, 161,  9,  9,  9,  9, 162, 18, 17, 18, 17, 18, 
    11, 12, 11, 12, 11, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 
    174, 175, 176, 177, 178, 179, 180, 174, 181, 182, 183, 12, 11, 12, 11, 12, 
    17, 18, 17, 18, 17, 184, 185, 186, 187, 188, 185, 189, 190, 191, 192, 193, 
    194, 195, 187, 196, 197, 188, 194, 198, 199, 200, 201, 18, 17, 18, 17, 18, 
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 
    202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 202, 
     9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
     9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9 , 9
  );

implementation

end.
