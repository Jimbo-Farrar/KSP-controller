// Handshake

void Handshake() {

  HPacket.id = 0;
  HPacket.M1 = 3;
  HPacket.M2 = 1;
  HPacket.M3 = 4;

  KSPBoardSendData(details(HPacket));
}





