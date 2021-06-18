function ready(fn) {
    if (document.readyState != 'loading') {
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
}

ready(function () {


    const divLog = document.querySelector("#log");
    fetch("/config.json")
        .then(response => response.json())
        .then(data => { console.log(data); divLog.innerHTML = JSON.stringify(data); });


    const formSetup = document.querySelector("form[name=setup]");

    formSetup.addEventListener("submit",async function (event) {
        event.preventDefault();

        const data = {
            mode: "normal",
            wifi_ssid: this["wifi_ssid"].value,
            wifi_password: this["wifi_password"].value,
            ntp_server: this["ntp_server"].value,
            ntp_gmt_offset: this["ntp_gmt_offset"].value
        };
        const options = {
            method: "POST",
            headers: {
                "Content-Type":
                    "application/json;charset=utf-8"
            },
            body: JSON.stringify(data)
        };
        const response = await fetch("/save_setup", options);
        if (response.ok) { // if HTTP-status is 200-299
            let json = await response.text();// json();
            alert("OK: " + json)
        } else {
            alert("HTTP-Error: " + response.status);
        }
    });

});
