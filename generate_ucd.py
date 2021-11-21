# Get the Unicode Character Database and generate necessary functions from it

"""
Autogenerate needed unicode codepoint classification functions from the UCD
"""

license = """
The Clear BSD License

Copyright (c) 2021 Valyrie Autumn
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the disclaimer
below) provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

     * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

     * Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
"""

import urllib.request, urllib.error
import argparse

# UCD version to be used (old versions might or might not work, dunno)
ucd_ver = "latest" # at time of development latest is ver 13

# canonical URL for the ucd
ucd_url = "https://www.unicode.org/Public/UCD/" + ucd_ver + "/ucd"

# the UCD is parsed according to UAX technical report 44 and accompanying
# materials, fetched from https://unicode.org/reports/tr44/

# TODO

# what properties am I interested in? (and from what files)
#  General_Category (UnicodeData.txt)
#  Prepend_Concatenation_Mark (PropList.txt)
#  Grapheme_Extend (DerivedCoreProperties.txt)
#  Emoji_Modifier (emoji-data.txt)
#  Regoinal_Indicator (PropList.txt)
#  Indic_Syllabic_Category (IndicSyllabicCategory.txt)
#  Grapheme_Cluster_Break (GraphemeBreakProperty.txt)
#  Hangul_Syllable_Type (HangulSyllableType.txt)

# TODO properties to tell if a character is a CJK character or not? This probably isn't helpful, as it will be font/terminal specific
#  East_Asian_Width
#  Ideographic

# get the resource behind a url

def fetchUrl(url):
     with urllib.request.urlopen(url) as response:
          return response.read()

# iterator to parse the generic UCD format

# TODO