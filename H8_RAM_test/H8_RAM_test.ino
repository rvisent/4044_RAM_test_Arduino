const int nClock4040 = 2;
const int clear4040 = 3;
const int dataInRAM = 5;
const int dataOutRAM = 6;
const int nWriteRAM = 8;
const int nCSRAM = 9;

const int RAMsize = 4096; // bits

void setup() {
  // put your setup code here, to run once:
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  // init pins direction and level
  digitalWrite(nClock4040, HIGH);
  pinMode(nClock4040, OUTPUT);
  digitalWrite(clear4040, LOW);
  pinMode(clear4040, OUTPUT);
  pinMode(dataInRAM, OUTPUT);
  digitalWrite(nWriteRAM, HIGH);
  pinMode(nWriteRAM, OUTPUT);
  digitalWrite(nCSRAM, HIGH);
  pinMode(nCSRAM, OUTPUT);
  pinMode(dataOutRAM, INPUT);
}

void reset4040()
{
  digitalWrite(clear4040, HIGH);
  delayMicroseconds(10);
  digitalWrite(clear4040, LOW);
  delayMicroseconds(5);
}

void inc4040()
{
  digitalWrite(nClock4040, LOW);
  delayMicroseconds(5);
  digitalWrite(nClock4040, HIGH);
  delayMicroseconds(10);
}

unsigned char RAMread()
{
  unsigned char value;
  digitalWrite(nWriteRAM, HIGH);
  digitalWrite(nCSRAM, LOW);
  delayMicroseconds(1);
  value = digitalRead(dataOutRAM) != 0;
  digitalWrite(nCSRAM, HIGH);
  return value;
}

unsigned char RAMwrite(unsigned char value)
{
  digitalWrite(dataInRAM, value);
  digitalWrite(nWriteRAM, LOW);
  digitalWrite(nCSRAM, LOW);
  delayMicroseconds(1);
  digitalWrite(nWriteRAM, HIGH);
  digitalWrite(nCSRAM, HIGH);
}

int cycles=0;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("cycle "); Serial.print(++cycles);
  Serial.print(" test 1: ");
  // reset address counter
  reset4040();

  unsigned char bError = 0;
  // alternate result left in each bit after test
  unsigned char toggle = 0;
  for (int idx=0; idx<RAMsize; idx++)
  {
    RAMwrite(toggle);
    if (RAMread() != toggle)
    {
      Serial.print("error on 1st write, addr="); Serial.print(idx); Serial.print(" value="); Serial.println(toggle);
      bError = 1;
      break;
    }

    // try opposite
    RAMwrite(!toggle);
    if (RAMread() != !toggle)
    {
      Serial.print("error on 2nd write, addr="); Serial.print(idx); Serial.print(" value="); Serial.println(!toggle);
      bError = 1;
      break;
    }

    // reverse toggle
    toggle = !toggle;

    // next address
    inc4040();
  }

  if (!bError)
    Serial.print("** passed ** - ");
  else
  {
    Serial.println("** failed **");
    return;
  }

  Serial.print("test 2: ");
  // restart
  reset4040();

  // check the last sequence 1-0-1-0...
  toggle = 1; bError = 0;
  for (int idx=0; idx<RAMsize; idx++)
  {
    if (RAMread() != toggle)
    {
      Serial.print("mismatch at addr="); Serial.print(idx); Serial.print(" expected="); Serial.println(toggle);
      bError = 1;
      break;
    }
    // reverse toggle
    toggle = !toggle;

    // next address
    inc4040();
  }

  if (!bError)
    Serial.print("** passed ** - ");
  else
  {
    Serial.println("** failed **");
    return;
  }

  Serial.print("test 3: ");
  // reset address counter
  reset4040();

  // write opposite sequence
  toggle = 0;
  for (int idx=0; idx<RAMsize; idx++)
  {
    RAMwrite(toggle);
    // reverse toggle
    toggle = !toggle;
    // next address
    inc4040();
  }

  // now check the sequence 0-1-0-1...
  toggle = 0; bError = 0;
  for (int idx=0; idx<RAMsize; idx++)
  {
    if (RAMread() != toggle)
    {
      Serial.print("mismatch at addr="); Serial.print(idx); Serial.print(" expected="); Serial.println(toggle);
      bError = 1;
      break;
    }
    // reverse toggle
    toggle = !toggle;

    // next address
    inc4040();
  }

  if (!bError)
    Serial.println("** passed **");
  else
  {
    Serial.println("** failed **");
    return;
  }

  // long pause
  delay(500);
}
