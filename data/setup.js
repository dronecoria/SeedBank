function ready(fn) {
    if (document.readyState != 'loading') {
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
}

let indexSensor = 0;

ready( async function () {

    const response = await fetch("/config.json");
    if(!response.ok){
        alert("config.json not found");
    }
    let configJson = await response.json();

    const divLog = document.querySelector("#log");
    divLog.innerHTML = JSON.stringify(configJson);

    const formSetup = document.querySelector("form[name=setup]");

    const addSensorButton = document.querySelector("#add_sensor");
    addSensorButton.addEventListener("click",function(event){
        addSensor();
    });



    //populate form
    for (const key in configJson) {
        if (Object.hasOwnProperty.call(configJson, key)) {
            const value = configJson[key];
            if(formSetup[key] !== undefined){
                if(key == "sensors"){
                    let index = 0;
                    configJson[key].forEach(sensor => {
                        addSensor(sensor.type,sensor.value);

                    });
                }else{
                    formSetup[key].value = configJson[key];
                }
            }
        }
    }



    formSetup.addEventListener("submit",async function (event) {
        event.preventDefault();

        let data = {
            "sensors": [],
            "actuators": []
        };
        const inputs = ['input', 'select'];//, 'button', 'textarea'];
        for (const key in formSetup) {
            if (Object.hasOwnProperty.call(formSetup, key)) {
                const el = formSetup[key];
                if (el.tagName != undefined && inputs.indexOf(el.tagName.toLowerCase()) !== -1){
                    if(el.dataset.sensor){
                        if (data["sensors"][el.dataset.sensor] === undefined) data["sensors"][el.dataset.sensor] = {};
                        data["sensors"][el.dataset.sensor][el.name] = el.value;
                    }else{
                        data[el.name] = el.value;
                    }

                }
            }
        }
        console.log(data);
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

    get_time();
    setInterval(get_time, 1 * 1000);
});

async function get_time(){
    const response = await fetch("/get_time");
    let time = await response.text();
    const output = document.querySelector("#time");
    output.innerHTML = time;
}


function addSensor(type="", value="") {
    const formSetup = document.querySelector("form[name=setup]");
    const templateSensor = document.querySelector("#sensors template");
    const clone = document.importNode(templateSensor.content, true);
    const select = clone.querySelector("select[name=type]");
    const input = clone.querySelector("input[name=value]");

    select.dataset.sensor = indexSensor;
    input.dataset.sensor = indexSensor;
    select.value = type;
    input.value = value;
    const delButton = clone.querySelector(".del_sensor");
    delButton.addEventListener("click", function (event) {
        event.target.parentNode.remove();
    });
    formSetup["sensors"].appendChild(clone);
    indexSensor++;
}
