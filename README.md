# Countering Relay and Spoofing Attacks in the Connection Establishment Phase of Wi-Fi Systems

To establish a secure Wi-Fi connection, a station first exchanges
several unprotected management frames with an access point (AP)
to eventually authenticate each other and install a pairwise key. It is,
therefore, possible for an adversary to spoof elements of those unprotected
frames at the physical (PHY) or MAC layers, facilitating
additional attacks (e.g., man-in-the-middle and starvation attacks).
Despite a few ad hoc efforts, there is still no practical way to counter
these attacks jointly. In this paper, we propose practical schemes to
employ cryptography at the PHY layer combined with a time-bound
technique to detect and mitigate such attacks in enterprise and
802.1X-based public networks. Our backward-compatible schemes
embed a digital signature of the AP (or a message authentication
code) in frame preamble signals and add only a negligible delay to
the connection establishment process and achieve a 98.9% true positive
rate in detecting an attacker who tries to relay valid preambles.
Furthermore, we conduct a formal security analysis of our scheme
using a model checker and a cryptographic protocol verifier and
evaluate its performance in a commercial AP-and-USRP testbed.

Naureen Hoque and Hanif Rahbari. 2023. Countering Relay and Spoofing
Attacks in the Connection Establishment Phase of Wi-Fi Systems. In Proceedings
of the 16th ACM Conference on Security and Privacy in Wireless and
Mobile Networks (WiSec ’23), May 29-June 1, 2023, Guildford, United Kingdom.
ACM, NewYork,NY, USA, 11 pages. https://doi.org/10.1145/3558482.3590185
