function ready(fn) {
    if (document.readyState != 'loading') {
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
}

let indexSensor = 0;
let indexTimetable = 0;

ready(async () => {

    const response = await fetch("/config.json");
    if (!response.ok) {
        alert("config.json not found");
    }
    let configJson = await response.json();

    const divLog = document.querySelector("#log");
    divLog.innerHTML = JSON.stringify(configJson);

    const formSetup = document.querySelector("form[name=setup]");

    const addSensorButton = document.querySelector("#add_sensor");
    addSensorButton.addEventListener("click", (event) => {
        addSensor();
    });

    const addTimetableButton = document.querySelector("#add_timetable");
    addTimetableButton.addEventListener("click", (event) => {
        addTimetable();
    });


    //populate form
    for (const key in configJson) {
        if (Object.hasOwnProperty.call(configJson, key)) {
            const value = configJson[key];
            if (formSetup[key] !== undefined) {
                if (key === "sensors") {
                    configJson[key].forEach(sensor => {
                        addSensor(sensor.type, sensor.value);
                    });
                } else if (key === "schedule") {
                    configJson[key].forEach(timetable => {
                        addTimetable(timetable.start, timetable.end,timetable.value);
                    });
                } else if (key === "heat") {
                    formSetup.querySelector("select[data-actuator=heat]").value = configJson[key].type;
                    formSetup.querySelector("input[data-actuator=heat]").value = configJson[key].value;
                } else if (key === "cold") {
                    formSetup.querySelector("select[data-actuator=cold]").value = configJson[key].type;
                    formSetup.querySelector("input[data-actuator=cold]").value = configJson[key].value;
                } else if (key === "fan") {
                    formSetup.querySelector("select[data-actuator=fan]").value = configJson[key].type;
                    formSetup.querySelector("input[data-actuator=fan]").value = configJson[key].value;
                }else if (key === "light") {
                    formSetup.querySelector("select[data-actuator=light]").value = configJson[key].type;
                    formSetup.querySelector("input[data-actuator=light]").value = configJson[key].value;
                } else {
                    formSetup[key].value = configJson[key];
                }
            }
        }
    }

    formSetup.addEventListener("submit", async (event) => {
        event.preventDefault();

        let data = {
            "sensors": [],
            "schedule": []
        };
        const inputs = ['input', 'select'];//, 'button', 'textarea'];
        for (const key in formSetup) {
            if (Object.hasOwnProperty.call(formSetup, key)) {
                const el = formSetup[key];
                if (el.tagName != undefined && inputs.indexOf(el.tagName.toLowerCase()) !== -1) {
                    if (el.dataset.sensor) {
                        if (data["sensors"][el.dataset.sensor] === undefined) data["sensors"][el.dataset.sensor] = {};
                        data["sensors"][el.dataset.sensor][el.name] = getValueInput(el);

                    } else if (el.dataset.actuator) {
                        if (data[el.dataset.actuator] === undefined) data[el.dataset.actuator] = {};
                        data[el.dataset.actuator][el.name] = getValueInput(el);

                    } else if (el.dataset.timetable) {
                        if (data["schedule"][el.dataset.timetable] === undefined){
                            data["schedule"][el.dataset.timetable] = {};
                        }

                        data["schedule"][el.dataset.timetable][el.name] = getValueInput(el);

                    } else {
                        data[el.name] = getValueInput(el);
                    }

                }
            }
        }
        // eliminate empty elements in array
        data["schedule"] = data["schedule"].filter(val => val);
        data["sensors"] =  data["sensors"].filter(val => val);

        //check schedule is correct, no gaps or overlaps
        //  1.- init = 00:00;
        //  2.- find init
        //  3.- check end > start  --> error
        //  4.- end is new init
        //  5.- repeat from 2 until end = 00:00
        //  6.- if steps != count(schedule)  --> error
        let checking = "00:00";
        let timetable = null;
        let count = 0;
        do {
            timetable = null;
            data["schedule"].forEach(t => {
                if (t.start == checking){
                    timetable = t;
                    count++;
                }
            });
            if(timetable == null){
                errorInSchedule("Gaps in schedule");
                return;
            }
            if (toMinutes(timetable.start,false) > toMinutes(timetable.end,true) ){
                errorInSchedule("timetravel");
                return;
            }
            checking = timetable.end;
        } while (timetable.end != "00:00");
        if (Object.keys(data["schedule"]).length != count ){
            errorInSchedule("Overlaps in schedule");
            return;
        }

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

function toMinutes(hour, isEnd)
{
    if(isEnd && hour == "00:00") hour = "24:00";
    let h = hour.split(":");
    return (h[0]*60) + (+h[1]);
}

function errorInSchedule(message)
{
    alert(message);
}

async function get_time() {
    const response = await fetch("/get_time");
    let time = await response.text();
    const output = document.querySelector("#time");
    output.innerHTML = time;
}

function getValueInput(input) {
    if (input.type == "number") {
        return +input.value;
    }
    return input.value;
}

function addSensor(type = "", value = "") {
    const formSetup = document.querySelector("form[name=setup]");
    const templateSensor = document.querySelector("#sensors template");
    const clone = document.importNode(templateSensor.content, true);
    const select = clone.querySelector("select[name=type]");
    const input = clone.querySelector("input[name=value]");

    select.dataset.sensor = indexSensor;
    input.dataset.sensor = indexSensor;
    select.value = type;
    input.value = value;
    const delButton = clone.querySelector(".delete");
    delButton.addEventListener("click", (event) => {
        event.target.parentNode.parentNode.remove();
    });
    formSetup["sensors"].appendChild(clone);
    indexSensor++;
}

function addTimetable(start = "", end = "", value = "") {
    const formSetup = document.querySelector("form[name=setup]");
    const templateTimetable = document.querySelector("#schedule template");
    const clone = document.importNode(templateTimetable.content, true);
    const input_start = clone.querySelector("input[name=start]");
    const input_end = clone.querySelector("input[name=end]");
    const input_value = clone.querySelector("input[name=value]");

    input_start.dataset.timetable = indexTimetable;
    input_end.dataset.timetable = indexTimetable;
    input_value.dataset.timetable = indexTimetable;

    input_start.value = start;
    input_end.value = end;
    input_value.value = value;

    const delButton = clone.querySelector(".delete");
    delButton.addEventListener("click", (event) => {
        event.target.parentNode.parentNode.remove();
    });
    formSetup["schedule"].appendChild(clone);
    indexTimetable++;
}
