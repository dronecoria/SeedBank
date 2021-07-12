let status = {};
let lastStatus = {};
function ready(fn) {
	if (document.readyState != 'loading') {
		fn();
	} else {
		document.addEventListener('DOMContentLoaded', fn);
	}
}

ready(async function () {

    getStatus();
    setInterval(getStatus, 3 * 1000);


});


async function getStatus() {

	const response = await fetch("/status.json");
	status = await response.json();

	const log = document.querySelector("#log");
	log.innerHTML = JSON.stringify(status);

    updateSensors();
    updateOthers();
    if ( JSON.stringify(status.actuators) != JSON.stringify(lastStatus.actuators)){
        updateActuators();
    }

    lastStatus = status;
}

function updateSensors()
{
    const sensors = document.querySelector("#sensors");
    sensors.innerHTML = "";
    const templateSensor = document.querySelector("#sensor");

    status.sensors.forEach(s => {
        const clone = document.importNode(templateSensor.content, true);

        clone.querySelector(".type").innerHTML = s.type;
        clone.querySelector(".value").innerHTML = s.value;

        sensors.appendChild(clone);
    });
}

function updateActuators()
{
    const actuators = document.querySelector("#actuators");
    actuators.innerHTML = "";
    const templateActuator = document.querySelector("#actuator");

    status.actuators.forEach(a => {
        const clone = document.importNode(templateActuator.content, true);

        clone.querySelector(".name").innerHTML = a.name;
        clone.querySelector(".type").innerHTML = a.type;
        const valueHTML = clone.querySelector(".value");

        if (a.type == "PWM") {
            valueHTML.type = "range";
            valueHTML.min = 0;
            valueHTML.max = 1;
            valueHTML.step = 0.01;
            valueHTML.value = a.value;
        } else {
            valueHTML.type = "checkbox";
            if(a.value){
                valueHTML.checked = "checked";
            }
        }

        if (status.handler != "test") {
            valueHTML.disabled = "disabled";
            valueHTML.value = a.value;
        }else{

            valueHTML.addEventListener("change", async (event) => {

                const value = (event.target.type == "checkbox") ? (+event.target.checked) : event.target.value;

                const data = { name: a.name, value: +(value) };
                const options = {
                    method: "POST",
                    headers: {
                        "Content-Type":
                            "application/json;charset=utf-8"
                    },
                    body: JSON.stringify(data)
                };
                const response = await fetch("/set_value", options);
                if (response.ok) {
                    lastStatus = {};
                }else{
                    alert("HTTP-Error: " + response.status);
                }
            });
        }
        actuators.appendChild(clone);
    });
}

function updateOthers()
{
    const others = document.querySelector("#others");
    others.innerHTML = "";
    const templateOthers = document.querySelector("#other");

    for (const key in status) {
        if (Object.hasOwnProperty.call(status, key)) {
            // if (key == "mode") {
            //
            // } else if (key == "handler") {
            // } else
            if (key != "actuators" && key != "sensors") {

                const clone = document.importNode(templateOthers.content, true);

                clone.querySelector(".name").innerHTML = key;
                clone.querySelector(".value").innerHTML = status[key];
                others.appendChild(clone);
            }
        }
    }
}
