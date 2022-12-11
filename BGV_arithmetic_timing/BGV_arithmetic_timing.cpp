/* Copyright (C) 2019-2021 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */

/*
 * Modified from https://github.com/homenc/HElib/blob/master/examples/BGV_packed_arithmetic/BGV_packed_arithmetic.cpp
 */

#include <iostream>

#include <helib/helib.h>

int main(int argc, char* argv[])
{

  // Plaintext prime modulus
  unsigned long p = 2;
  // Cyclotomic polynomial - defines phi(m)
  unsigned long m = 31775;
  // Hensel lifting (default = 1)
  unsigned long r = 1;
  // Number of bits of the modulus chain
  unsigned long bits = 580;
  // Number of columns of Key-Switching matrix (default = 2 or 3)
  unsigned long c = 2;
  unsigned long t = 64;
  std::vector<long> mvec = {41, 775};
  std::vector<long> gens = {6976, 24806};
  std::vector<long> ords = {40, 30};

  std::cout << "Initialising context object..." << std::endl;
  // Initialize context
  // This object will hold information about the algebra created from the
  // previously set parameters
  helib::Context context = helib::ContextBuilder<helib::BGV>()
                               .m(m)
                               .p(p)
                               .r(r)
                               .gens(gens)
                               .ords(ords)
                               .bits(bits)
                               .c(c)
                               .bootstrappable(true)
                               .skHwt(t)
                               .mvec(mvec)
                               .thickboot()
                               .build();

  // Print the context
  context.printout();
  std::cout << std::endl;

  // Print the security level
  std::cout << "Security: " << context.securityLevel() << std::endl;

  // Secret key management
  std::cout << "Creating secret key..." << std::endl;
  // Create a secret key associated with the context
  helib::SecKey secret_key(context);
  // Generate the secret key
  secret_key.GenSecKey();
  std::cout << "Generating key-switching matrices..." << std::endl;
  // Compute key-switching matrices that we need
  helib::addSome1DMatrices(secret_key);
  helib::addFrbMatrices(secret_key);

  // Generate bootstrapping data
  secret_key.genRecryptData();

  // Public key management
  // Set the secret key (upcast: SecKey is a subclass of PubKey)
  const helib::PubKey& public_key = secret_key;

  // Get the EncryptedArray of the context
  const helib::EncryptedArray& ea = context.getEA();

  // Get the number of slot (phi(m))
  long nslots = ea.size();
  std::cout << "Number of slots: " << nslots << std::endl;

  // Create a vector of long with nslots elements
  helib::Ptxt<helib::BGV> ptxt(context);
  // Set it with numbers 0..nslots - 1
  // ptxt = [0] [1] [2] ... [nslots-2] [nslots-1]
  for (int i = 0; i < ptxt.size(); ++i) {
    ptxt[i] = i;
  }

  // Create a ciphertext object
  helib::Ctxt ctxt(public_key);
  // Encrypt the plaintext using the public_key
  public_key.Encrypt(ctxt, ptxt);

  /********** Operations **********/
  // Cypertext multiplication
  helib::resetAllTimers();
  ctxt.multiplyBy(ctxt);
  std::cout << "Cyphertext multiplication\n";
  helib::printNamedTimer(std::cout, "multiplyBy");
  helib::printNamedTimer(std::cout, "BluesteinFFT");
  // Bootstrapping (thickboot)
  helib::resetAllTimers();
  public_key.reCrypt(ctxt);
  std::cout << "Bootstrapping (thickboot)\n";
  helib::printNamedTimer(std::cout, "reCrypt");
  helib::printNamedTimer(std::cout, "BluesteinFFT");
  // Bootstrapping (thinboot)
  helib::resetAllTimers();
  public_key.thinReCrypt(ctxt);
  std::cout << "Bootstrapping (thinboot)\n";
  helib::printNamedTimer(std::cout, "thinReCrypt");
  helib::printNamedTimer(std::cout, "BluesteinFFT");

  return 0;
}
