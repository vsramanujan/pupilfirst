[@bs.config {jsx: 3}];

open CurriculumEditor__Types;

let str = React.string;

type visibility =
  | Hidden
  | Visible(selection, saving)
and selection =
  | Nothing
  | EmbedBlock
and saving = bool;

type state = {
  visibility,
  errorMessage: option(string),
  embedUrl: string,
};

type action =
  | MakeVisible
  | Hide
  | SelectEmbedBlock
  | UpdateEmbedUrl(string)
  | Save(selection)
  | SetError(string);

let reducer = (state, action) =>
  switch (action) {
  | MakeVisible => {...state, visibility: Visible(Nothing, false)}
  | Hide => {...state, visibility: Hidden}
  | SelectEmbedBlock => {...state, visibility: Visible(EmbedBlock, false)}
  | UpdateEmbedUrl(embedUrl) => {...state, embedUrl}
  | Save(selection) => {...state, visibility: Visible(selection, false)}
  | SetError(error) => {...state, errorMessage: Some(error)}
  };

type button =
  | MarkdownButton
  | FileButton
  | ImageButton
  | EmbedButton;

let containerClasses = (visibility, forceVisible) => {
  let classes = "add-content-block py-3 cursor-pointer";

  classes
  ++ (
    switch (forceVisible, visibility) {
    | (true, _)
    | (false, Visible(_)) => " add-content-block--open"
    | (false, Hidden) => ""
    }
  );
};

let createContentBlock =
    (
      formData,
      target,
      state,
      dispatch,
      blockType,
      sortIndex,
      createNewContentCB,
    ) =>
  Api.sendFormData(
    "/school/targets/" ++ (target |> Target.id) ++ "/content_block",
    formData,
    json => {()},
    () => (),
  );

let faIcons = (blockType: ContentBlock.blockType) =>
  switch (blockType) {
  | Markdown(_markdown) => React.null
  | File(_url, _title, _filename) =>
    <i className="fas fa-file text-6xl text-gray-500" />
  | Image(_url, _caption) =>
    <i className="fas fa-image text-6xl text-gray-500" />
  | Embed(_url, _embedCode) =>
    [|
      <i
        key="youtube-icon"
        className="fab fa-youtube text-6xl text-gray-500 px-2"
      />,
      <i
        key="slideshare-icon"
        className="fab fa-slideshare text-6xl text-gray-500 px-2"
      />,
      <i
        key="vimeo-icon"
        className="fab fa-vimeo text-6xl text-gray-500 px-2"
      />,
    |]
    |> React.array
  };

let handleFileUpload = (dispatch, event, blockType) => {
  switch (ReactEvent.Form.target(event)##files) {
  | [||] => dispatch(SetError("No file selected"))
  | files =>
    let file = files[0];
    let maxFileSize = 5 * 1024 * 1024;
    let error =
      file##size > maxFileSize
        ? Some("The maximum file size is 5 MB. Please select another file.")
        : None;
    switch (error) {
    | Some(errorMessage) =>
      dispatch(SetError(errorMessage));
      Notification.error("Upload Error", errorMessage);
    | None => dispatch(SetError("Failed to upload " ++ file##name))
    };
  };
};

let contentUploadContainer = (blockType, dispatch, state, editorId) =>
  <div
    className="content-block__content-placeholder flex flex-col justify-center text-center p-10">
    <div> {faIcons(blockType)} </div>
    <p className="text-xs text-gray-800 mt-1">
      {(
         switch (blockType) {
         | Markdown(_markdown) => ""
         | File(_url, _title, _filename) => "You can upload PDF, JPG, ZIP etc."
         | Image(_url, _caption) => "You can upload PNG, JPG, GIF files"
         | Embed(_url, _embedCode) => "Paste in a URL to embed"
         }
       )
       |> str}
    </p>
    <div className="mt-2">
      <input
        id={"content-block-editor__file-input-" ++ editorId}
        type_="file"
        className="hidden"
        required=false
        multiple=false
        name="content_block[file]"
        onChange={event => handleFileUpload(dispatch, event, blockType)}
      />
    </div>
  </div>;

let submitForm =
    (
      event,
      state,
      authenticityToken,
      blockType,
      dispatch,
      target,
      sortIndex,
      createNewContentCB,
      selection,
    ) => {
  dispatch(Save(selection));
  ReactEvent.Form.preventDefault(event);
  let element =
    ReactEvent.Form.target(event) |> DomUtils.EventTarget.unsafeToElement;

  let formData = DomUtils.FormData.create(element);

  createContentBlock(
    formData,
    target,
    state,
    dispatch,
    blockType,
    sortIndex,
    createNewContentCB,
  );
};

let button = (sortIndex, onClick, send, createContentBlockCB, button) => {
  let (faIcon, buttonText) =
    switch (button) {
    | MarkdownButton => ("fab fa-markdown", "Markdown")
    | FileButton => ("far fa-file-alt", "File")
    | ImageButton => ("far fa-image", "Image")
    | EmbedButton => ("fas fa-code", "Embed")
    };

  <div
    className="add-content-block__block-content-type-picker px-3 pt-4 pb-3 flex-1 text-center text-primary-200"
    onClick={_e => onClick(send, createContentBlockCB)}>
    <i className={faIcon ++ " text-2xl"} />
    <p className="font-semibold"> {buttonText |> str} </p>
  </div>;
};

let createMarkdownBlock = (send, createContentBlockCB) => {
  ();
};

let acceptImageFile = (send, createContentBlockCB) => {
  ();
};

let showEmbedBlockForm = (send, createContentBlockCB) => {
  ();
};

let acceptFile = (send, createContentBlockCB) => {
  ();
};

[@react.component]
let make = (~sortIndex, ~forceVisible, ~createContentBlockCB) => {
  let (state, send) =
    React.useReducer(
      reducer,
      {visibility: Hidden, errorMessage: None, embedUrl: ""},
    );

  <form
    id={"create-content-block-" ++ (sortIndex |> string_of_int)}
    className={containerClasses(state.visibility, forceVisible)}>
    {forceVisible
       /* Spacer for add-content-block section */
       ? <div className="h-10" />
       : <div
           className="add-content-block__plus-button-container relative"
           onClick={_event => send(MakeVisible)}>
           <div
             id={"add-block-" ++ (sortIndex |> string_of_int)}
             title="Add block"
             className="add-content-block__plus-button bg-gray-200 hover:bg-gray-300 relative rounded-lg border border-gray-500 w-10 h-10 flex justify-center items-center mx-auto z-20">
             <i
               className="fas fa-plus text-base add-content-block__plus-button-icon"
             />
           </div>
         </div>}
    <div
      className="add-content-block__block-content-type text-sm hidden shadow-lg mx-auto relative bg-primary-900 rounded-lg -mt-4 z-10"
      id={"content-type-picker-" ++ (sortIndex |> string_of_int)}>
      {button(
         sortIndex,
         createMarkdownBlock,
         send,
         createContentBlockCB,
         MarkdownButton,
       )}
      {button(
         sortIndex,
         acceptImageFile,
         send,
         createContentBlockCB,
         ImageButton,
       )}
      {button(
         sortIndex,
         showEmbedBlockForm,
         send,
         createContentBlockCB,
         EmbedButton,
       )}
      {button(sortIndex, acceptFile, send, createContentBlockCB, FileButton)}
    </div>
  </form>;
};
