# robko01_retrofit_fw



## Setup the environment variables 
**This commands are for Windows terminal only.**

Setup the WiFi SSID name.
```sh
    setx WIFI_SSID "\"YOUR_SSID\"" /m
```

Setup the WiFi password.
```sh
    setx YOUR_PASS "\"YOUR_PASS\"" /m
```

OTA password hash.
```sh
    setx OTA_PASS_HASH "\"OTA_PASS_HASH\"" /m
```

Wireguard endpoint IP address.
```sh
    setx WG_ENDPOINT "\"WG_ENDPOINT\"" /m
```

Wireguard local IP address.
```sh
    setx WG_LOCAL_IP "\"WG_LOCAL_IP\"" /m
```

Wireguard your private key.
```sh
    setx WG_PRIVATE_KEY "\"WG_PRIVATE_KEY\"" /m
```

Wireguard server public key.
```sh
    setx WG_PUBLIC_KEY "\"WG_PUBLIC_KEY\"" /m
```

## Notes

[Doc](https://docs.platformio.org/en/latest/envvars.html) specification how to use environment variables in build time.

[Article](https://community.platformio.org/t/providing-credentials-without-the-keys-being-displayed-in-the-code/32437/3) that describe how to use commands described previous:


