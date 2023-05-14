//https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API
//also 
//https://github.com/ongzzzzzz/p5.web-serial

var log = "";
let port;
let reader;
let inputDone;
let outputDone;
let inputStream;
let outputStream;

var BAUDRATE = 9600; //115200;
async function connect() {
  // - Request a port and open a connection.
  port = await navigator.serial.requestPort();
  // - Wait for the port to open.
  await port.open({ baudRate: BAUDRATE });

  let decoder = new TextDecoderStream();
  inputDone = port.readable.pipeTo(decoder.writable);
  inputStream = decoder.readable;

  reader = inputStream.getReader();
  readLoop();

  const encoder = new TextEncoderStream();
  outputDone = encoder.readable.pipeTo(port.writable);
  outputStream = encoder.writable;
}

async function readLoop() {
  // CODELAB: Add read loop here.
  while (true) {
    const { value, done } = await reader.read();
    if (value) {
      //console.log(value);
      log += value;
    }
    if (done) {
      console.log('[readLoop] DONE', done);
      reader.releaseLock();
      break;
    }
  }
}

function writeToStream(...lines) {
  // CODELAB: Write to output stream
  // CODELAB: Write to output stream
  const writer = outputStream.getWriter();
  lines.forEach((line) => {
    console.log('[SEND]', line);
    writer.write(line + '\r');
  });
  writer.releaseLock();
}
