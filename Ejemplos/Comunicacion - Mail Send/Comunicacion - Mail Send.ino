// Include ESP Mail Client library (this library)
#include <ESP_Mail_Client.h>


// Define the global used SMTP Session object which used for SMTP transsport
SMTPSession smtp;

// Define the global used session config data which used to store the TCP session configuration
ESP_Mail_Session session;

void setup()
{

  Serial.begin(115200);

  WiFi.begin("<ssid>", "<password>");
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set the session config
  session.server.host_name = "smtp.office365.com"; // for outlook.com
  session.server.port = 587;
  session.login.email = "your Email address"; // set to empty for no SMTP Authentication
  session.login.password = "your Email password"; // set to empty for no SMTP Authentication
  session.login.user_domain = "client domain or ip e.g. mydomain.com";

  // Set the NTP config time
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;

  // Define the SMTP_Message class variable to handle to message being transport
  SMTP_Message message;

  // Set the message headers
  message.sender.name = "My Mail";
  message.sender.email = "sender or your Email address";
  message.subject = "Test sending Email";
  message.addRecipient("name1", "email1");
  message.addRecipient("name2", "email2");

  message.addCc("email3");
  message.addBcc("email4");

  // Set the message content
  message.text.content = "This is simple plain text message";

  //Base64 data of image
  const char *greenImg = "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAAoUlEQVR42u"
                         "3RAQ0AMAgAoJviyWxtAtNYwzmoQGT/eqwRQoQgRAhChCBECEKECBGCECEI"
                         "EYIQIQgRghCECEGIEIQIQYgQhCBECEKEIEQIQoQgBCFCECIEIUIQIgQhCB"
                         "GCECEIEYIQIQhBiBCECEGIEIQIQQhChCBECEKEIEQIQhAiBCFCECIEIUIQ"
                         "ghAhCBGCECEIEYIQIUKEIEQIQoQg5LoBBaDPbQYiMoMAAAAASUVORK5CYII=";

  // Define the attachment data
  SMTP_Attachment att;

  // Set the attatchment info
  att.descr.filename = "green.png";
  att.descr.mime = "image/png";
  att.blob.data = (uint8_t *)greenImg;
  att.blob.size = strlen(greenImg);
  // Set the transfer encoding to base64
  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  // We set the content encoding to match the above greenImage data
  att.descr.content_encoding = Content_Transfer_Encoding::enc_base64;

  // Add attachment to the message
  message.addAttachment(att);

  // Connect to server with the session config
  smtp.connect(&session);

  // Start sending Email and close the session
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());

}

Read Email
The following code will read the latest email.

// Include ESP Mail Client library (this library)
#include <ESP_Mail_Client.h>


// Define the global used IMAP Session object which used for IMAP transsport
IMAP_Config config;


// Define the global used session config data which used to store the TCP session configuration
ESP_Mail_Session session;


void setup()
{

  Serial.begin(115200);

  WiFi.begin("<ssid>", "<password>");
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set the session config
  session.server.host_name = "outlook.office365.com"; //for outlook.com
  session.server.port = 993;
  session.login.email = "your Email address";
  session.login.password = "your Email password";

  // Define the config class variable for searching or fetching operation and store the messsagess data
  IMAP_Config config;

  
  // Set to enable the message content which will be stored in the IMAP_Config data
  config.enable.html = true;
  config.enable.text = true;


  // Connect to the server with the defined session and options
  imap.connect(&session, &config);

  // Open or select the mailbox folder to read the message
  imap.selectFolder("INBOX");


  // Define the message UID (number) which required to fetch or read the message
  // In this case we will get the UID from the max message number (lastest message)
  // then imap.getUID and imap.selectedFolder().msgCount() should be called after 
  // calling select or open the folder (mailbox).
  config.fetch.uid = imap.getUID(imap.selectedFolder().msgCount());

  // Define the empty search criteria to disable the messsage search
  config.search.criteria.clear();


  // Read the Email and close the session
  MailClient.readMail(&imap);


  // Get the message(s) list
  IMAP_MSG_List msgList = imap.data();

  // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
  // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
  // In ESP32 and ESP32, you can use Serial.printf directly.

  for (size_t i = 0; i < msgList.msgItems.size(); i++)
  {
    // Iterate to get each message data through the message item data
    IMAP_MSG_Item msg = msgList.msgItems[i];

    Serial.println("################################");
    ESP_MAIL_PRINTF("Messsage Number: %s\n", msg.msgNo);
    ESP_MAIL_PRINTF("Messsage UID: %s\n", msg.UID);
    ESP_MAIL_PRINTF("Messsage ID: %s\n", msg.ID);
    ESP_MAIL_PRINTF("Accept Language: %s\n", msg.acceptLang);
    ESP_MAIL_PRINTF("Content Language: %s\n", msg.contentLang);
    ESP_MAIL_PRINTF("From: %s\n", msg.from);
    ESP_MAIL_PRINTF("From Charset: %s\n", msg.fromCharset);
    ESP_MAIL_PRINTF("To: %s\n", msg.to);
    ESP_MAIL_PRINTF("To Charset: %s\n", msg.toCharset);
    ESP_MAIL_PRINTF("CC: %s\n", msg.cc);
    ESP_MAIL_PRINTF("CC Charset: %s\n", msg.ccCharset);
    ESP_MAIL_PRINTF("Date: %s\n", msg.date);
    ESP_MAIL_PRINTF("Subject: %s\n", msg.subject);
    ESP_MAIL_PRINTF("Subject Charset: %s\n", msg.subjectCharset);

    // If the message body is available
    if (!imap.headerOnly())
    {
      ESP_MAIL_PRINTF("Text Message: %s\n", msg.text.content);
      ESP_MAIL_PRINTF("Text Message Charset: %s\n", msg.text.charSet);
      ESP_MAIL_PRINTF("Text Message Transfer Encoding: %s\n", msg.text.transfer_encoding);
      ESP_MAIL_PRINTF("HTML Message: %s\n", msg.html.content);
      ESP_MAIL_PRINTF("HTML Message Charset: %s\n", msg.html.charSet);
      ESP_MAIL_PRINTF("HTML Message Transfer Encoding: %s\n\n", msg.html.transfer_encoding);
    }
  }

}
