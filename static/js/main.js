document.getElementById('transform_button').addEventListener('click', function(){
  const inputText = document.getElementById('input_text').value;
  
  if (!inputText.trim()){
      document.getElementById('output_text').value = "Please press button with input data";
    return;
  }
  const data ={
    assemblerCode: inputText,
    options:{
      architecture: "x64",
      mode: "64bit"
    }
  };

  const currentUrl = window.location.href;

  fetch(currentUrl, {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
})
.then(response => response.json())
.then(result => {
    document.getElementById('output_text').value = result.data;
})
.catch(error => {
    document.getElementById('output_text').value = "Some error occurs!\n" + error;
});

});

document.addEventListener('DOMContentLoaded',function(){
  document.getElementById('input_text').value =`section .text
  global _start
                                   
_start:
  push 0x21646c72
  mov rcx, 0x6f77206f6c6c6548
  push rcx
  mov rsi, rsp
  mov rdi, 1
  mov rax, 1
  mov rdx, 13
  syscall`;
})